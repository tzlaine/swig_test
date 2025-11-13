#include "Urepl_graph.h"

// Adapted from replication graph from
// https://github.com/locus84/LocusReplicationGraph; their copyright follows.

// MIT License
//
// Copyright(c) 2019 Locus
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files(the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish , distribute, sublicense,
// and / or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <Engine/LevelScriptActor.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <UObject/UObjectIterator.h>

#if 0 // WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerCategoryReplicator.h"
#endif


namespace {
    void remove_connection_from_team(
        std::map<int, std::vector<Urepl_graph_conn *>> & map,
        int team,
        Urepl_graph_conn * conn)
    {
        auto const it = map.find(team);
        if (it == map.end())
            return;
        auto & [_, vec] = *it;
        std::erase(vec, conn);
        if (vec.empty())
            map.erase(it);
    }
}

Urepl_graph::Urepl_graph()
{
    ReplicationConnectionManagerClass = Urepl_graph_conn::StaticClass();

    FClassReplicationInfoPreset PawnClassRepInfo;
    PawnClassRepInfo.class_ = APawn::StaticClass();
    PawnClassRepInfo.DistancePriorityScale = 1.f;
    PawnClassRepInfo.StarvationPriorityScale = 1.f;
    PawnClassRepInfo.ActorChannelFrameTimeout = 4;
    // small size of cull distance squard leads inconsistant cull becuase of
    // distance between actual character and viewposition keep it bigger than
    // distance between actual pawn and inviewer
    PawnClassRepInfo.CullDistanceSquared = 15000.f * 15000.f;
    ReplicationInfoSettings.Add(PawnClassRepInfo);
}

void InitClassReplicationInfo(
    FClassReplicationInfo & Info,
    UClass * class_,
    bool bSpatialize,
    float ServerMaxTickRate)
{
    AActor * CDO = class_->GetDefaultObject<AActor>();
    if (bSpatialize) {
        Info.SetCullDistanceSquared(CDO->NetCullDistanceSquared);
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("Setting cull distance for %s to %f (%f)"),
            *class_->GetName(),
            Info.GetCullDistanceSquared(),
            FMath::Sqrt(Info.GetCullDistanceSquared()));
    }

    Info.ReplicationPeriodFrame = FMath::Max<uint32>(
        (uint32)FMath::RoundToFloat(
            ServerMaxTickRate / CDO->NetUpdateFrequency),
        1);

    UClass * NativeClass = class_;
    while (!NativeClass->IsNative() && NativeClass->GetSuperClass() &&
           NativeClass->GetSuperClass() != AActor::StaticClass()) {
        NativeClass = NativeClass->GetSuperClass();
    }

    UE_LOG(
        LogReplicationGraph,
        Log,
        TEXT("Setting replication period for %s (%s) to %d frames (%.2f)"),
        *class_->GetName(),
        *NativeClass->GetName(),
        Info.ReplicationPeriodFrame,
        CDO->NetUpdateFrequency);
}

const UClass * GetParentNativeClass(const UClass * class_)
{
    while (class_ && !class_->IsNative()) {
        class_ = class_->GetSuperClass();
    }

    return class_;
}

void Urepl_graph::InitGlobalActorClassSettings()
{
    Super::InitGlobalActorClassSettings();

    auto AddInfo = [&](UClass * class_, Erepl_node_kind routing) {
        class_to_routing_.Set(class_, routing);
    };

    AddInfo(
        AReplicationGraphDebugActor::StaticClass(),
        Erepl_node_kind::none); // Not needed. Replicated special case inside
                                // RepGraph
    AddInfo(
        AInfo::StaticClass(),
        Erepl_node_kind::always); // Non spatialized, relevant to all
    AddInfo(
        ALevelScriptActor::StaticClass(),
        Erepl_node_kind::none); // Not needed
#if 0 /* WITH_GAMEPLAY_DEBUGGER */
    AddInfo(
        AGameplayDebuggerCategoryReplicator::StaticClass(),
        Erepl_node_kind::connection); // Only owner connection viable
#endif

    for (FClassReplicationPolicyPreset PolicyBP : ReplicationPolicySettings) {
        if (PolicyBP.class_) {
            AddInfo(PolicyBP.class_, PolicyBP.routing);
        }
    }

    // this does contains all replicated class except GetIsReplicated is false
    // actor if someone need to make replication work, mark it as replicated
    // and control it over replication graph
    TArray<UClass *> AllReplicatedClasses;

    // Iterate all class
    for (TObjectIterator<UClass> It; It; ++It) {
        UClass * class_ = *It;
        AActor * ActorCDO = Cast<AActor>(class_->GetDefaultObject());

        if (!ActorCDO || !ActorCDO->GetIsReplicated())
            continue;

        // Skip SKEL and REINST classes.
        if (class_->GetName().StartsWith(TEXT("SKEL_")) ||
            class_->GetName().StartsWith(TEXT("REINST_"))) {
            continue;
        }

        // --------------------------------------------------------------------
        // This is a replicated class. Save this off for the second pass below
        // --------------------------------------------------------------------

        AllReplicatedClasses.Add(class_);

        // Skip if already in the map (added explicitly)
        if (class_to_routing_.Contains(class_, false))
            continue;

        auto ShouldSpatialize = [](const AActor * CDO) {
            return CDO->GetIsReplicated() &&
                   (!(CDO->bAlwaysRelevant || CDO->bOnlyRelevantToOwner ||
                      CDO->bNetUseOwnerRelevancy));
        };

        auto GetLegacyDebugStr = [](const AActor * CDO) {
            return FString::Printf(
                TEXT("%s [%d/%d/%d]"),
                *CDO->GetClass()->GetName(),
                CDO->bAlwaysRelevant,
                CDO->bOnlyRelevantToOwner,
                CDO->bNetUseOwnerRelevancy);
        };

        // Only handle this class if it differs from its super. There is no need
        // to put every child class explicitly in the graph class mapping
        UClass * SuperClass = class_->GetSuperClass();
        if (AActor * SuperCDO = Cast<AActor>(SuperClass->GetDefaultObject())) {
            if (SuperCDO->GetIsReplicated() == ActorCDO->GetIsReplicated() &&
                SuperCDO->bAlwaysRelevant == ActorCDO->bAlwaysRelevant &&
                SuperCDO->bOnlyRelevantToOwner ==
                    ActorCDO->bOnlyRelevantToOwner &&
                SuperCDO->bNetUseOwnerRelevancy ==
                    ActorCDO->bNetUseOwnerRelevancy) {
                // same settings with superclass, ignore this class
                continue;
            }
        }

        if (ShouldSpatialize(ActorCDO)) {
            AddInfo(class_, Erepl_node_kind::dynamic_spatial);
        } else if (
            ActorCDO->bAlwaysRelevant && !ActorCDO->bOnlyRelevantToOwner) {
            AddInfo(class_, Erepl_node_kind::always);
        } else if (ActorCDO->bOnlyRelevantToOwner) {
            AddInfo(class_, Erepl_node_kind::connection);
        }

        // TODO:: currently missing feature, !bAlwaysRelevant &&
        // bOnlyRelevantToOwner -> only owner see this but is spatialized
    }

    TArray<FClassReplicationInfoPreset> ValidClassReplicationInfoPreset;
    // custom setting
    for (FClassReplicationInfoPreset & ReplicationInfoBP :
         ReplicationInfoSettings) {
        if (ReplicationInfoBP.class_) {
            GlobalActorReplicationInfoMap.SetClassInfo(
                ReplicationInfoBP.class_,
                ReplicationInfoBP.CreateClassReplicationInfo());
            ValidClassReplicationInfoPreset.Add(ReplicationInfoBP);
        }
    }

    UReplicationGraphNode_ActorListFrequencyBuckets::DefaultSettings.ListSize =
        12;

    // Set FClassReplicationInfo based on legacy settings from all replicated
    // classes
    for (UClass * ReplicatedClass : AllReplicatedClasses) {
        if (FClassReplicationInfoPreset * Preset =
                ValidClassReplicationInfoPreset.FindByPredicate(
                    [&](const FClassReplicationInfoPreset & Info) {
                        return ReplicatedClass->IsChildOf(Info.class_.Get());
                    })) {
            // duplicated or set included child will be ignored
            if (Preset->class_.Get() == ReplicatedClass ||
                Preset->IncludeChildClasses) {
                continue;
            }
        }

        FClassReplicationInfo ClassInfo;
        InitClassReplicationInfo(
            ClassInfo,
            ReplicatedClass,
            spatial(class_to_routing_.GetChecked(ReplicatedClass)),
            NetDriver->NetServerMaxTickRate);
        GlobalActorReplicationInfoMap.SetClassInfo(ReplicatedClass, ClassInfo);
    }


    // Print out what we came up with
    UE_LOG(LogReplicationGraph, Log, TEXT(""));
    UE_LOG(LogReplicationGraph, Log, TEXT("Class Routing Map: "));
    UEnum * Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("Erepl_node_kind"));
    for (auto it = class_to_routing_.CreateIterator(); it;
         ++it) {
        UClass * class_ =
            CastChecked<UClass>(it.Key().ResolveObjectPtr());
        const Erepl_node_kind routing = it.Value();

        // Only print if different than native class
        UClass * ParentNativeClass = GetParentNativeClass(class_);
        const Erepl_node_kind * parent_routing =
            class_to_routing_.Get(ParentNativeClass);
        if (parent_routing && class_ != ParentNativeClass &&
            routing == *parent_routing) {
            continue;
        }
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("  %s (%s) -> %s"),
            *class_->GetName(),
            *GetNameSafe(ParentNativeClass),
            *Enum->GetNameStringByValue(static_cast<uint32>(routing)));
    }

    UE_LOG(LogReplicationGraph, Log, TEXT(""));
    UE_LOG(LogReplicationGraph, Log, TEXT("Class Settings Map: "));
    for (auto ClassRepInfoIt =
             GlobalActorReplicationInfoMap.CreateClassMapIterator();
         ClassRepInfoIt;
         ++ClassRepInfoIt) {
        const UClass * class_ =
            CastChecked<UClass>(ClassRepInfoIt.Key().ResolveObjectPtr());
        const FClassReplicationInfo & ClassInfo = ClassRepInfoIt.Value();
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("  %s (%s) -> %s"),
            *class_->GetName(),
            *GetNameSafe(GetParentNativeClass(class_)),
            *ClassInfo.BuildDebugStringDelta());
    }

    // Rep destruct infos based on CVar value
    DestructInfoMaxDistanceSquared =
        DestructionInfoMaxDistance * DestructionInfoMaxDistance;

#if 0 // WITH_GAMEPLAY_DEBUGGER
    AGameplayDebuggerCategoryReplicator::NotifyDebuggerOwnerChange.AddUObject(
        this, &Urepl_graph::OnGameplayDebuggerOwnerChange);
#endif
}

void Urepl_graph::InitGlobalGraphNodes()
{
    // -----------------------------------------------
    //	Spatial Actors
    // -----------------------------------------------

    GridNode = CreateNewNode<UReplicationGraphNode_GridSpatialization2D>();
    GridNode->CellSize = SpacialCellSize;
    GridNode->SpatialBias = SpatialBias;

    if (!EnableSpatialRebuilds) {
        GridNode->AddToClassRebuildDenyList(
            AActor::StaticClass()); // Disable All spatial rebuilding
    }

    AddGlobalGraphNode(GridNode);

    // -----------------------------------------------
    //	Always Relevant (to everyone) Actors
    // -----------------------------------------------
    AlwaysRelevantNode =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_WithPending>();
    AddGlobalGraphNode(AlwaysRelevantNode);
}

void Urepl_graph::InitConnectionGraphNodes(
    UNetReplicationGraphConnection * RepGraphConnection)
{
    Super::InitConnectionGraphNodes(RepGraphConnection);

    // UE_LOG(LogReplicationGraph, Warning, TEXT("InitConnection : %s"),
    // *RepGraphConnection->NetConnection->PlayerController->GetName());
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(RepGraphConnection);
    if (!conn) {
        UE_LOG(
            LogReplicationGraph,
            Warning,
            TEXT("Unrecognized ConnectionDriver class, Expected "
                 "Urepl_graph_conn"));
    }

    conn->AlwaysRelevantForConnectionNode =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForConnection>();
    AddConnectionGraphNode(
        conn->AlwaysRelevantForConnectionNode, RepGraphConnection);

    conn->TeamConnectionNode =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForTeam>();
    AddConnectionGraphNode(conn->TeamConnectionNode, RepGraphConnection);

    // don't care about team names as it's initial value is always no_team
}

void Urepl_graph::OnRemoveConnectionGraphNodes(
    UNetReplicationGraphConnection * RepGraphConnection)
{
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(RepGraphConnection);
    if (conn) {
        if (conn->team != no_team) {
            remove_connection_from_team(team_to_conn_, conn->team, conn);
        }
    }
}

void Urepl_graph::RemoveClientConnection(
    UNetConnection * NetConnection)
{
    // we completely override super function

    int32 ConnectionId = 0;
    bool found = false;

    // Remove the RepGraphConnection associated with this NetConnection. Also
    // update ConnectionIds to stay compact.
    auto UpdateList = [&](TArray<UNetReplicationGraphConnection *> List) {
        for (int32 idx = 0; idx < Connections.Num(); ++idx) {
            UNetReplicationGraphConnection * ConnectionManager =
                Connections[idx];
            repCheck(ConnectionManager);

            if (ConnectionManager->NetConnection == NetConnection) {
                ensure(!found);
                // Nofity this to handle something - remove from team list
                OnRemoveConnectionGraphNodes(ConnectionManager);
                Connections.RemoveAtSwap(idx, 1, false);
                found = true;
            } else {
                ConnectionManager->ConnectionOrderNum = ConnectionId++;
            }
        }
    };

    UpdateList(Connections);
    UpdateList(PendingConnections);

    if (!found) {
        UE_LOG(
            LogReplicationGraph,
            Warning,
            TEXT("UReplicationGraph::RemoveClientConnection could not find "
                 "connection in Connection (%d) or PendingConnections (%d) "
                 "lists"),
            Connections.Num(),
            PendingConnections.Num());
    }
}

void Urepl_graph::RouteAddNetworkActorToNodes(
    const FNewReplicatedActorInfo & ActorInfo,
    FGlobalActorReplicationInfo & GlobalInfo)
{
    Erepl_node_kind routing = routing_for(ActorInfo.Class);
    switch (routing) {
    case Erepl_node_kind::none:
        break;
    case Erepl_node_kind::always:
        AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
        break;
    case Erepl_node_kind::connection:
    case Erepl_node_kind::team:
        RouteAddNetworkActorToConnectionNodes(routing, ActorInfo, GlobalInfo);
        break;
    case Erepl_node_kind::static_spatial:
        GridNode->AddActor_Static(ActorInfo, GlobalInfo);
        break;
    case Erepl_node_kind::dynamic_spatial:
        GridNode->AddActor_Dynamic(ActorInfo, GlobalInfo);
        break;
    case Erepl_node_kind::dormant_spatial:
        GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);
        break;
    };
}

void Urepl_graph::RouteRemoveNetworkActorToNodes(
    const FNewReplicatedActorInfo & ActorInfo)
{
    Erepl_node_kind routing = routing_for(ActorInfo.Class);

    switch (routing) {
    case Erepl_node_kind::none:
        break;
    case Erepl_node_kind::always:
        AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
        break;
    case Erepl_node_kind::connection:
    case Erepl_node_kind::team:
        RouteRemoveNetworkActorToConnectionNodes(routing, ActorInfo);
        break;
    case Erepl_node_kind::static_spatial:
        GridNode->RemoveActor_Static(ActorInfo);
        break;
    case Erepl_node_kind::dynamic_spatial:
        GridNode->RemoveActor_Dynamic(ActorInfo);
        break;
    case Erepl_node_kind::dormant_spatial:
        GridNode->RemoveActor_Dormancy(ActorInfo);
        break;
    };
}

// this function will be called seamless map transition
// as all actors will be removed in silly order, we have to deal with it
void Urepl_graph::ResetGameWorldState()
{
    Super::ResetGameWorldState();

    // all actor will be destroyed. just reset it.
    pending_actors_.clear();
    pending_team_requests_.clear();
#pragma warning(push)
#pragma warning(disable : 4458)
    auto EmptyConnectionNode =
        [](TArray<UNetReplicationGraphConnection *> & Connections) {
            for (UNetReplicationGraphConnection * conn_base : Connections) {
                if (Urepl_graph_conn * conn =
                        Cast<Urepl_graph_conn>(conn_base)) {
                    conn->AlwaysRelevantForConnectionNode
                        ->NotifyResetAllNetworkActors();
                }
            }
        };
#pragma warning(pop)
    EmptyConnectionNode(PendingConnections);
    EmptyConnectionNode(Connections);

    // as connection does not destroyed, we keep it
    // team_to_conn_.clear();
}

// Since we listen to global (static) events, we need to watch out for cross
// world broadcasts (PIE)
#if WITH_EDITOR
#define CHECK_WORLDS(X)                                                        \
    if (X->GetWorld() != GetWorld())                                           \
        return;
#else
#define CHECK_WORLDS(X)
#endif

void Urepl_graph::AddDependentActor(
    AActor * ReplicatorActor, AActor * DependentActor)
{
    if (ReplicatorActor && DependentActor) {
        CHECK_WORLDS(ReplicatorActor);

        if (FGlobalActorReplicationInfo * ReplicationInfo =
                GlobalActorReplicationInfoMap.Find(ReplicatorActor)) {
            if (!ReplicationInfo->GetDependentActorList().Contains(
                    DependentActor)) {
                GlobalActorReplicationInfoMap.AddDependentActor(
                    ReplicatorActor, DependentActor);
            }
        } else {
            UE_LOG(
                LogReplicationGraph,
                Warning,
                TEXT("ReplicatorActor privided is not replicating"));
        }
    }
}

void Urepl_graph::RemoveDependentActor(
    AActor * ReplicatorActor, AActor * DependentActor)
{
    if (ReplicatorActor && DependentActor) {
        CHECK_WORLDS(ReplicatorActor);

        if (FGlobalActorReplicationInfo * ReplicationInfo =
                GlobalActorReplicationInfoMap.Find(ReplicatorActor)) {
            GlobalActorReplicationInfoMap.RemoveDependentActor(
                ReplicatorActor, DependentActor);
        } else {
            UE_LOG(
                LogReplicationGraph,
                Warning,
                TEXT("ReplicatorActor privided is not replicating"));
        }
    }
}

void Urepl_graph::change_owner_to(
    AActor * ActorToChange, AActor * NewOwner)
{
    Erepl_node_kind routing = routing_for(ActorToChange->GetClass());
    if (!ActorToChange || routing == Erepl_node_kind::none ||
        spatial(routing)) {
        // routing doesn't matter for chaning owner
        return;
    }

    // remove from previous connection specific nodes.
    RouteRemoveNetworkActorToConnectionNodes(
        routing, FNewReplicatedActorInfo(ActorToChange));

    // change owner safely
    ActorToChange->SetOwner(NewOwner);

    // re-route to connection specific nodes with new owner
    FGlobalActorReplicationInfo & GlobalInfo =
        GlobalActorReplicationInfoMap.Get(ActorToChange);
    RouteAddNetworkActorToConnectionNodes(
        routing, FNewReplicatedActorInfo(ActorToChange), GlobalInfo);
}

void Urepl_graph::set_team_for(
    APlayerController * pc, int next_team)
{
    if (!pc)
        return;
    if (Urepl_graph_conn * conn = find_connection_graph(pc)) {
        int curr_team = conn->team;
        if (curr_team != next_team) {
            if (curr_team != no_team) {
                remove_connection_from_team(
                    team_to_conn_, curr_team, conn);
            }

            if (next_team != no_team)
                team_to_conn_[next_team].push_back(conn);

            conn->team = next_team;
        }
    } else {
        pending_team_requests_.push_back(team_request{next_team, pc});
    }
}

void Urepl_graph::RouteAddNetworkActorToConnectionNodes(
    Erepl_node_kind routing,
    const FNewReplicatedActorInfo & ActorInfo,
    FGlobalActorReplicationInfo & GlobalInfo)
{
    if (Urepl_graph_conn * conn =
            find_connection_graph(ActorInfo.GetActor())) {
        switch (routing) {
        case Erepl_node_kind::connection:
            conn->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(
                ActorInfo);
            break;
        case Erepl_node_kind::team:
            conn->TeamConnectionNode->NotifyAddNetworkActor(ActorInfo);
            break;
        };
    } else if (ActorInfo.Actor->GetNetOwner()) {
        // this actor is not yet ready. add to pending array to handle pending
        // route
        pending_actors_.push_back(ActorInfo.GetActor());
    }
}


void Urepl_graph::RouteRemoveNetworkActorToConnectionNodes(
    Erepl_node_kind routing, const FNewReplicatedActorInfo & ActorInfo)
{
    if (Urepl_graph_conn * conn =
            find_connection_graph(ActorInfo.GetActor())) {
        switch (routing) {
        case Erepl_node_kind::connection:
            conn->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(
                ActorInfo);
            break;
        case Erepl_node_kind::team:
            conn->TeamConnectionNode->NotifyRemoveNetworkActor(ActorInfo);
            break;
        };
    } else if (ActorInfo.Actor->GetNetOwner()) {
        // this actor is not yet ready. but doesn't matter the pending array
        // contains the actor or not
        std::erase(pending_actors_, ActorInfo.GetActor());
    }
}

void Urepl_graph::process_pendings()
{
    if (!pending_team_requests_.empty()) {
        std::vector TempRequests = std::move(pending_team_requests_);

        for (auto & Request : TempRequests) {
            if (Request.pc && Request.pc->IsValidLowLevel()) {
                // if failed, it will automatically re-added to pending list
                set_team_for(Request.pc, Request.team);
            }
        }
    }

    if (!pending_actors_.empty()) {
        std::vector TempActors = std::move(pending_actors_);

        for (AActor * Actor : TempActors) {
            if (Actor && Actor->IsValidLowLevel()) {
                if (UNetConnection * Connection = Actor->GetNetConnection()) {
                    // if failed, it will automatically re-added to pending list
                    Erepl_node_kind routing =
                        routing_for(Actor->GetClass());
                    FGlobalActorReplicationInfo & GlobalInfo =
                        GlobalActorReplicationInfoMap.Get(Actor);
                    RouteAddNetworkActorToConnectionNodes(
                        routing, FNewReplicatedActorInfo(Actor), GlobalInfo);
                }
            }
        }
    }
}


class Urepl_graph_conn *
Urepl_graph::find_connection_graph(const AActor * a)
{
    if (!a)
        return nullptr;

    if (UNetConnection * NetConnection = a->GetNetConnection()) {
        if (Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(
                FindOrAddConnectionManager(NetConnection))) {
            return conn;
        }
    }

    return nullptr;
}

#if 0 // WITH_GAMEPLAY_DEBUGGER
void Urepl_graph::OnGameplayDebuggerOwnerChange(
    AGameplayDebuggerCategoryReplicator * Debugger,
    APlayerController * OldOwner)
{
    if (Urepl_graph_conn * conn =
            find_connection_graph(OldOwner)) {
        FNewReplicatedActorInfo ActorInfo(Debugger);
        conn->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(
            ActorInfo);
    }

    if (Urepl_graph_conn * conn =
            find_connection_graph(Debugger->GetReplicationOwner())) {
        FNewReplicatedActorInfo ActorInfo(Debugger);
        conn->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(
            ActorInfo);
    }
}
#endif

void Urepl_graph::print_rep_node_kinds()
{
    UEnum * Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("Erepl_node_kind"));
    if (!Enum) {
        return;
    }

    GLog->Logf(TEXT("===================================="));
    GLog->Logf(TEXT("Shooter Replication Routing Policies"));
    GLog->Logf(TEXT("===================================="));

    for (auto It = class_to_routing_.CreateIterator(); It; ++It) {
        FObjectKey ObjKey = It.Key();

        Erepl_node_kind routing = It.Value();

        GLog->Logf(
            TEXT("%-40s --> %s"),
            *GetNameSafe(ObjKey.ResolveObjectPtr()),
            *Enum->GetNameStringByValue(static_cast<uint32>(routing)));
    }
}

Erepl_node_kind Urepl_graph::routing_for(UClass * class_)
{
    Erepl_node_kind * PolicyPtr = class_to_routing_.Get(class_);
    Erepl_node_kind routing = PolicyPtr ? *PolicyPtr : Erepl_node_kind::none;
    return routing;
}

void UReplicationGraphNode_AlwaysRelevant_ForTeam::
    GatherActorListsForConnection(
        const FConnectionGatherActorListParameters & Params)
{
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(&Params.ConnectionManager);
    if (conn && conn->team != no_team) {
        Urepl_graph * ReplicationGraph =
            Cast<Urepl_graph>(GetOuter());
        auto const it = ReplicationGraph->team_to_conn_.find(conn->team);
        if (it != ReplicationGraph->team_to_conn_.end()) {
            for (Urepl_graph_conn * TeamMember : it->second) {
                // we call parent
                TeamMember->TeamConnectionNode
                    ->GatherActorListsForConnectionDefault(Params);
            }
        }
    } else {
        Super::GatherActorListsForConnection(Params);
    }
}

UReplicationGraphNode_AlwaysRelevant_WithPending::
    UReplicationGraphNode_AlwaysRelevant_WithPending()
{
    bRequiresPrepareForReplicationCall = true;
}

void UReplicationGraphNode_AlwaysRelevant_WithPending::PrepareForReplication()
{
    Urepl_graph * ReplicationGraph =
        Cast<Urepl_graph>(GetOuter());
    ReplicationGraph->process_pendings();
}

void UReplicationGraphNode_AlwaysRelevant_ForTeam::
    GatherActorListsForConnectionDefault(
        const FConnectionGatherActorListParameters & Params)
{
    Super::GatherActorListsForConnection(Params);
}

// console commands copied from shooter repgraph
//  ------------------------------------------------------------------------------

FAutoConsoleCommandWithWorldAndArgs ShooterPrintRepNodePoliciesCmd(
    TEXT("ReplGraph.PrintRouting"),
    TEXT("Prints how actor classes are routed to RepGraph nodes"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString> & Args, UWorld * World) {
            for (TObjectIterator<Urepl_graph> It; It; ++It) {
                It->print_rep_node_kinds();
            }
        }));


FAutoConsoleCommandWithWorldAndArgs ChangeFrequencyBucketsCmd(
    TEXT("ReplGraph.FrequencyBuckets"),
    TEXT("Resets frequency bucket count."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString> & Args, UWorld * World) {
            int32 Buckets = 1;
            if (Args.Num() > 0) {
                LexTryParseString<int32>(Buckets, *Args[0]);
            }

            UE_LOG(
                LogReplicationGraph,
                Display,
                TEXT("Setting Frequency Buckets to %d"),
                Buckets);
            for (TObjectIterator<
                     UReplicationGraphNode_ActorListFrequencyBuckets> It;
                 It;
                 ++It) {
                UReplicationGraphNode_ActorListFrequencyBuckets * Node = *It;
                Node->SetNonStreamingCollectionSize(Buckets);
            }
        }));
