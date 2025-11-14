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

    void populate_class_repl_info(
        FClassReplicationInfo & info,
        UClass * class_,
        bool spatial,
        float server_max_tick_rate)
    {
        // TODO: Visibility should be based on detection and stealth numbers
        // for the actors in question; follow something along the lines of the
        // Google response to "ue5 c++ how to use replicatioon graphs to
        // implement units with different detection radii" for a possible
        // approach.
        AActor * cdo = class_->GetDefaultObject<AActor>();
        if (spatial) {
            info.SetCullDistanceSquared(cdo->NetCullDistanceSquared);
            UE_LOG(
                LogReplicationGraph,
                Log,
                TEXT("Setting cull distance for %s to %f (%f)"),
                *class_->GetName(),
                info.GetCullDistanceSquared(),
                FMath::Sqrt(info.GetCullDistanceSquared()));
        }

        info.ReplicationPeriodFrame = std::max(
            (uint32)FMath::RoundToFloat(
                server_max_tick_rate / cdo->NetUpdateFrequency),
            1u);

        UClass * native_class = class_;
        while (!native_class->IsNative() && native_class->GetSuperClass() &&
               native_class->GetSuperClass() != AActor::StaticClass()) {
            native_class = native_class->GetSuperClass();
        }

        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("Setting replication period for %s (%s) to %d frames (%.2f)"),
            *class_->GetName(),
            *native_class->GetName(),
            info.ReplicationPeriodFrame,
            cdo->NetUpdateFrequency);
    }

    UClass const * parent_native_class_of(UClass const * class_)
    {
        while (class_ && !class_->IsNative()) {
            class_ = class_->GetSuperClass();
        }
        return class_;
    }
}

Urepl_graph::Urepl_graph()
{
    ReplicationConnectionManagerClass = Urepl_graph_conn::StaticClass();

    FClassReplGraphInfo repl_info;
    repl_info.class_ = APawn::StaticClass();
    repl_info.DistancePriorityScale = 1.f;
    repl_info.StarvationPriorityScale = 1.f;
    repl_info.ActorChannelFrameTimeout = 4;
    // small size of cull distance squard leads inconsistant cull because of
    // distance between actual character and viewposition keep it bigger than
    // distance between actual pawn and viewer
    repl_info.CullDistanceSquared = 15000.f * 15000.f;
    class_repl_infos_.Add(repl_info);
}

void Urepl_graph::InitGlobalActorClassSettings()
{
    Super::InitGlobalActorClassSettings();

    auto add_routing = [&](UClass * class_, Erepl_node_kind routing) {
        class_to_routing_.Set(class_, routing);
    };

    add_routing(
        AReplicationGraphDebugActor::StaticClass(),
        Erepl_node_kind::none); // Not needed. Replicated special case inside
                                // RepGraph
    add_routing(
        AInfo::StaticClass(),
        Erepl_node_kind::always); // Non spatialized, relevant to all
    add_routing(
        ALevelScriptActor::StaticClass(),
        Erepl_node_kind::none); // Not needed
#if 0                           /* WITH_GAMEPLAY_DEBUGGER */
    add_routing(
        AGameplayDebuggerCategoryReplicator::StaticClass(),
        Erepl_node_kind::connection); // Only owner connection viable
#endif

    for (FClassReplGraphRouting routing : class_routings_) {
        if (!routing.class_)
            continue;
        add_routing(routing.class_, routing.routing);
    }

    // this contains all replicated classes except those for which
    // GetIsReplicated() is false
    TArray<UClass *> all_repl_classes;

    // Iterate all class
    for (TObjectIterator<UClass> it; it; ++it) {
        UClass * class_ = *it;
        AActor * actor_cdo = Cast<AActor>(class_->GetDefaultObject());

        if (!actor_cdo || !actor_cdo->GetIsReplicated())
            continue;

        // Skip SKEL and REINST classes.
        if (class_->GetName().StartsWith(TEXT("SKEL_")) ||
            class_->GetName().StartsWith(TEXT("REINST_"))) {
            continue;
        }

        // --------------------------------------------------------------------
        // This is a replicated class. Save this off for the second pass below
        // --------------------------------------------------------------------

        all_repl_classes.Add(class_);

        // Skip if already in the map (added explicitly)
        if (class_to_routing_.Contains(class_, false))
            continue;

        auto ShouldSpatialize = [](AActor const * cdo) {
            return cdo->GetIsReplicated() &&
                   (!(cdo->bAlwaysRelevant || cdo->bOnlyRelevantToOwner ||
                      cdo->bNetUseOwnerRelevancy));
        };

        auto GetLegacyDebugStr = [](AActor const * cdo) {
            return FString::Printf(
                TEXT("%s [%d/%d/%d]"),
                *cdo->GetClass()->GetName(),
                cdo->bAlwaysRelevant,
                cdo->bOnlyRelevantToOwner,
                cdo->bNetUseOwnerRelevancy);
        };

        // Only handle this class if it differs from its super. There is no need
        // to put every child class explicitly in the graph class mapping
        UClass * SuperClass = class_->GetSuperClass();
        if (AActor * super_cdo = Cast<AActor>(SuperClass->GetDefaultObject())) {
            if (super_cdo->GetIsReplicated() == actor_cdo->GetIsReplicated() &&
                super_cdo->bAlwaysRelevant == actor_cdo->bAlwaysRelevant &&
                super_cdo->bOnlyRelevantToOwner ==
                    actor_cdo->bOnlyRelevantToOwner &&
                super_cdo->bNetUseOwnerRelevancy ==
                    actor_cdo->bNetUseOwnerRelevancy) {
                // same settings with superclass, ignore this class
                continue;
            }
        }

        if (ShouldSpatialize(actor_cdo)) {
            add_routing(class_, Erepl_node_kind::dynamic_spatial);
        } else if (
            actor_cdo->bAlwaysRelevant && !actor_cdo->bOnlyRelevantToOwner) {
            add_routing(class_, Erepl_node_kind::always);
        } else if (actor_cdo->bOnlyRelevantToOwner) {
            add_routing(class_, Erepl_node_kind::connection);
        }

        // TODO:: currently missing feature, !bAlwaysRelevant &&
        // bOnlyRelevantToOwner -> only owner see this but is spatialized
    }

    TArray<FClassReplGraphInfo> class_repl_infos;
    // custom setting
    for (FClassReplGraphInfo & repl_info : class_repl_infos_) {
        if (repl_info.class_) {
            GlobalActorReplicationInfoMap.SetClassInfo(
                repl_info.class_, repl_info.class_repl_info());
            class_repl_infos.Add(repl_info);
        }
    }

    UReplicationGraphNode_ActorListFrequencyBuckets::DefaultSettings.ListSize =
        12;

    // Set FClassReplicationInfo based on legacy settings from all replicated
    // classes
    for (UClass * repl_class : all_repl_classes) {
        if (FClassReplGraphInfo * info =
                class_repl_infos.FindByPredicate(
                    [&](FClassReplGraphInfo const & Info) {
                        return repl_class->IsChildOf(Info.class_.Get());
                    })) {
            // duplicated or set included child will be ignored
            if (info->class_.Get() == repl_class ||
                info->IncludeChildClasses) {
                continue;
            }
        }

        FClassReplicationInfo class_info;
        populate_class_repl_info(
            class_info,
            repl_class,
            spatial(class_to_routing_.GetChecked(repl_class)),
            NetDriver->NetServerMaxTickRate);
        GlobalActorReplicationInfoMap.SetClassInfo(repl_class, class_info);
    }


    // Print out what we came up with
    UE_LOG(LogReplicationGraph, Log, TEXT(""));
    UE_LOG(LogReplicationGraph, Log, TEXT("Class Routing Map: "));
    UEnum * Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("Erepl_node_kind"));
    for (auto it = class_to_routing_.CreateIterator(); it; ++it) {
        UClass * class_ = CastChecked<UClass>(it.Key().ResolveObjectPtr());
        Erepl_node_kind const routing = it.Value();

        // Only print if different than native class
        UClass * parent_native_class = GetParentNativeClass(class_);
        Erepl_node_kind const * parent_routing =
            class_to_routing_.Get(parent_native_class);
        if (parent_routing && class_ != parent_native_class &&
            routing == *parent_routing) {
            continue;
        }
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("  %s (%s) -> %s"),
            *class_->GetName(),
            *GetNameSafe(parent_native_class),
            *Enum->GetNameStringByValue(static_cast<uint32>(routing)));
    }

    UE_LOG(LogReplicationGraph, Log, TEXT(""));
    UE_LOG(LogReplicationGraph, Log, TEXT("Class Settings Map: "));
    for (auto it = GlobalActorReplicationInfoMap.CreateClassMapIterator(); it;
         ++it) {
        UClass const * class_ =
            CastChecked<UClass>(it.Key().ResolveObjectPtr());
        FClassReplicationInfo const & class_info = it.Value();
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("  %s (%s) -> %s"),
            *class_->GetName(),
            *GetNameSafe(parent_native_class_of(class_)),
            *class_info.BuildDebugStringDelta());
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
    UNetReplicationGraphConnection * repl_graph_conn)
{
    Super::InitConnectionGraphNodes(repl_graph_conn);

    // UE_LOG(LogReplicationGraph, Warning, TEXT("InitConnection : %s"),
    // *repl_graph_conn->NetConnection->PlayerController->GetName());
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(repl_graph_conn);
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
        conn->AlwaysRelevantForConnectionNode, repl_graph_conn);

    conn->TeamConnectionNode =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForTeam>();
    AddConnectionGraphNode(conn->TeamConnectionNode, repl_graph_conn);

    // don't care about team names as it's initial value is always no_team
}

void Urepl_graph::OnRemoveConnectionGraphNodes(
    UNetReplicationGraphConnection * repl_graph_conn)
{
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(repl_graph_conn);
    if (conn) {
        if (conn->team != no_team) {
            remove_connection_from_team(team_to_conn_, conn->team, conn);
        }
    }
}

void Urepl_graph::RemoveClientConnection(UNetConnection * net_conn)
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

            if (ConnectionManager->NetConnection == net_conn) {
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
    FNewReplicatedActorInfo const & actor_info,
    FGlobalActorReplicationInfo & global_info)
{
    Erepl_node_kind routing = routing_for(actor_info.Class);
    switch (routing) {
    case Erepl_node_kind::none: break;
    case Erepl_node_kind::always:
        AlwaysRelevantNode->NotifyAddNetworkActor(actor_info);
        break;
    case Erepl_node_kind::connection:
    case Erepl_node_kind::team:
        RouteAddNetworkActorToConnectionNodes(routing, actor_info, global_info);
        break;
    case Erepl_node_kind::static_spatial:
        GridNode->AddActor_Static(actor_info, global_info);
        break;
    case Erepl_node_kind::dynamic_spatial:
        GridNode->AddActor_Dynamic(actor_info, global_info);
        break;
    case Erepl_node_kind::dormant_spatial:
        GridNode->AddActor_Dormancy(actor_info, global_info);
        break;
    };
}

void Urepl_graph::RouteRemoveNetworkActorToNodes(
    FNewReplicatedActorInfo const & actor_info)
{
    Erepl_node_kind routing = routing_for(actor_info.Class);

    switch (routing) {
    case Erepl_node_kind::none: break;
    case Erepl_node_kind::always:
        AlwaysRelevantNode->NotifyRemoveNetworkActor(actor_info);
        break;
    case Erepl_node_kind::connection:
    case Erepl_node_kind::team:
        RouteRemoveNetworkActorToConnectionNodes(routing, actor_info);
        break;
    case Erepl_node_kind::static_spatial:
        GridNode->RemoveActor_Static(actor_info);
        break;
    case Erepl_node_kind::dynamic_spatial:
        GridNode->RemoveActor_Dynamic(actor_info);
        break;
    case Erepl_node_kind::dormant_spatial:
        GridNode->RemoveActor_Dormancy(actor_info);
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

void Urepl_graph::change_owner_to(AActor * a, AActor * owner)
{
    Erepl_node_kind routing = routing_for(a->GetClass());
    if (!a || routing == Erepl_node_kind::none || spatial(routing)) {
        // routing doesn't matter for chaning owner
        return;
    }

    // remove from previous connection specific nodes.
    RouteRemoveNetworkActorToConnectionNodes(
        routing, FNewReplicatedActorInfo(a));

    // change owner safely
    a->SetOwner(owner);

    // re-route to connection specific nodes with new owner
    FGlobalActorReplicationInfo & global_info =
        GlobalActorReplicationInfoMap.Get(a);
    RouteAddNetworkActorToConnectionNodes(
        routing, FNewReplicatedActorInfo(a), global_info);
}

void Urepl_graph::set_team_for(APlayerController * pc, int next_team)
{
    if (!pc)
        return;
    if (Urepl_graph_conn * conn = find_connection_graph(pc)) {
        int curr_team = conn->team;
        if (curr_team != next_team) {
            if (curr_team != no_team) {
                remove_connection_from_team(team_to_conn_, curr_team, conn);
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
    FNewReplicatedActorInfo const & actor_info,
    FGlobalActorReplicationInfo & global_info)
{
    if (Urepl_graph_conn * conn =
            find_connection_graph(actor_info.GetActor())) {
        switch (routing) {
        case Erepl_node_kind::connection:
            conn->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(
                actor_info);
            break;
        case Erepl_node_kind::team:
            conn->TeamConnectionNode->NotifyAddNetworkActor(actor_info);
            break;
        };
    } else if (actor_info.Actor->GetNetOwner()) {
        // this actor is not yet ready. add to pending array to handle pending
        // route
        pending_actors_.push_back(actor_info.GetActor());
    }
}


void Urepl_graph::RouteRemoveNetworkActorToConnectionNodes(
    Erepl_node_kind routing, FNewReplicatedActorInfo const & actor_info)
{
    if (Urepl_graph_conn * conn =
            find_connection_graph(actor_info.GetActor())) {
        switch (routing) {
        case Erepl_node_kind::connection:
            conn->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(
                actor_info);
            break;
        case Erepl_node_kind::team:
            conn->TeamConnectionNode->NotifyRemoveNetworkActor(actor_info);
            break;
        };
    } else if (actor_info.Actor->GetNetOwner()) {
        // this actor is not yet ready. but doesn't matter the pending array
        // contains the actor or not
        std::erase(pending_actors_, actor_info.GetActor());
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
                    Erepl_node_kind routing = routing_for(Actor->GetClass());
                    FGlobalActorReplicationInfo & global_info =
                        GlobalActorReplicationInfoMap.Get(Actor);
                    RouteAddNetworkActorToConnectionNodes(
                        routing, FNewReplicatedActorInfo(Actor), global_info);
                }
            }
        }
    }
}


Urepl_graph_conn * Urepl_graph::find_connection_graph(AActor const * a)
{
    if (!a)
        return nullptr;

    if (UNetConnection * net_conn = a->GetNetConnection()) {
        if (Urepl_graph_conn * conn =
                Cast<Urepl_graph_conn>(FindOrAddConnectionManager(net_conn))) {
            return conn;
        }
    }

    return nullptr;
}

#if 0 // WITH_GAMEPLAY_DEBUGGER
void Urepl_graph::OnGameplayDebuggerOwnerChange(
    AGameplayDebuggerCategoryReplicator * debugger,
    APlayerController * pc)
{
    if (Urepl_graph_conn * conn = find_connection_graph(pc)) {
        FNewReplicatedActorInfo actor_info(debugger);
        conn->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(
            actor_info);
    }

    if (Urepl_graph_conn * conn =
            find_connection_graph(debugger->GetReplicationOwner())) {
        FNewReplicatedActorInfo actor_info(debugger);
        conn->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(
            actor_info);
    }
}
#endif

void Urepl_graph::print_rep_node_kinds()
{
    UEnum * enum_ = FindObject<UEnum>(ANY_PACKAGE, TEXT("Erepl_node_kind"));
    if (!enum_)
        return;

    GLog->Logf(TEXT("===================================="));
    GLog->Logf(TEXT("Shooter Replication Routing Policies"));
    GLog->Logf(TEXT("===================================="));

    for (auto it = class_to_routing_.CreateIterator(); it; ++it) {
        FObjectKey ObjKey = it.Key();

        Erepl_node_kind routing = it.Value();

        GLog->Logf(
            TEXT("%-40s --> %s"),
            *GetNameSafe(ObjKey.ResolveObjectPtr()),
            *enum_->GetNameStringByValue(static_cast<uint32>(routing)));
    }
}

Erepl_node_kind Urepl_graph::routing_for(UClass * class_)
{
    auto * const ptr = class_to_routing_.Get(class_);
    return ptr ? *ptr : Erepl_node_kind::none;
}

void UReplicationGraphNode_AlwaysRelevant_ForTeam::
    GatherActorListsForConnection(
        FConnectionGatherActorListParameters const & params)
{
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(&params.ConnectionManager);
    if (conn && conn->team != no_team) {
        Urepl_graph * repl_graph = Cast<Urepl_graph>(GetOuter());
        auto const it = repl_graph->team_to_conn_.find(conn->team);
        if (it != repl_graph->team_to_conn_.end()) {
            for (Urepl_graph_conn * team_conn : it->second) {
                team_conn->TeamConnectionNode
                    ->GatherActorListsForConnectionDefault(params);
            }
        }
    } else {
        Super::GatherActorListsForConnection(params);
    }
}

UReplicationGraphNode_AlwaysRelevant_WithPending::
    UReplicationGraphNode_AlwaysRelevant_WithPending()
{
    bRequiresPrepareForReplicationCall = true;
}

void UReplicationGraphNode_AlwaysRelevant_WithPending::PrepareForReplication()
{
    Cast<Urepl_graph>(GetOuter())->process_pendings();
}

void UReplicationGraphNode_AlwaysRelevant_ForTeam::
    GatherActorListsForConnectionDefault(
        FConnectionGatherActorListParameters const & params)
{
    Super::GatherActorListsForConnection(params);
}

// console commands copied from shooter repgraph
//  ------------------------------------------------------------------------------

FAutoConsoleCommandWithWorldAndArgs ShooterPrintRepNodePoliciesCmd(
    TEXT("ReplGraph.PrintRouting"),
    TEXT("Prints how actor classes are routed to RepGraph nodes"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](TArray<FString> const & args, UWorld *) {
            for (TObjectIterator<Urepl_graph> it; it; ++it) {
                it->print_rep_node_kinds();
            }
        }));


FAutoConsoleCommandWithWorldAndArgs ChangeFrequencyBucketsCmd(
    TEXT("ReplGraph.FrequencyBuckets"),
    TEXT("Resets frequency bucket count."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](TArray<FString> const & args, UWorld *) {
            int32 buckets = 1;
            if (!args.IsEmpty())
                LexTryParseString<int32>(buckets, *args[0]);

            UE_LOG(
                LogReplicationGraph,
                Display,
                TEXT("Setting Frequency Buckets to %d"),
                buckets);
            for (TObjectIterator<
                     UReplicationGraphNode_ActorListFrequencyBuckets> it;
                 it;
                 ++it) {
                UReplicationGraphNode_ActorListFrequencyBuckets * Node = *it;
                Node->SetNonStreamingCollectionSize(buckets);
            }
        }));
