#include "Ureplication_graph.h"

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


DEFINE_LOG_CATEGORY(LogLocusReplicationGraph);

ULocusReplicationGraph::ULocusReplicationGraph()
{
    ReplicationConnectionManagerClass =
        ULocusReplicationConnectionGraph::StaticClass();

    FClassReplicationInfoPreset PawnClassRepInfo;
    PawnClassRepInfo.Class = APawn::StaticClass();
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
    UClass * Class,
    bool bSpatialize,
    float ServerMaxTickRate)
{
    AActor * CDO = Class->GetDefaultObject<AActor>();
    if (bSpatialize) {
        Info.SetCullDistanceSquared(CDO->NetCullDistanceSquared);
        UE_LOG(
            LogLocusReplicationGraph,
            Log,
            TEXT("Setting cull distance for %s to %f (%f)"),
            *Class->GetName(),
            Info.GetCullDistanceSquared(),
            FMath::Sqrt(Info.GetCullDistanceSquared()));
    }

    Info.ReplicationPeriodFrame = FMath::Max<uint32>(
        (uint32)FMath::RoundToFloat(
            ServerMaxTickRate / CDO->NetUpdateFrequency),
        1);

    UClass * NativeClass = Class;
    while (!NativeClass->IsNative() && NativeClass->GetSuperClass() &&
           NativeClass->GetSuperClass() != AActor::StaticClass()) {
        NativeClass = NativeClass->GetSuperClass();
    }

    UE_LOG(
        LogLocusReplicationGraph,
        Log,
        TEXT("Setting replication period for %s (%s) to %d frames (%.2f)"),
        *Class->GetName(),
        *NativeClass->GetName(),
        Info.ReplicationPeriodFrame,
        CDO->NetUpdateFrequency);
}

const UClass * GetParentNativeClass(const UClass * Class)
{
    while (Class && !Class->IsNative()) {
        Class = Class->GetSuperClass();
    }

    return Class;
}

void ULocusReplicationGraph::InitGlobalActorClassSettings()
{
    Super::InitGlobalActorClassSettings();

    auto AddInfo = [&](UClass * Class, Erepl_node_kind Mapping) {
        ClassRepNodePolicies.Set(Class, Mapping);
    };

    AddInfo(
        AReplicationGraphDebugActor::StaticClass(),
        Erepl_node_kind::none); // Not needed. Replicated special case
                                    // inside RepGraph
    AddInfo(
        AInfo::StaticClass(),
        Erepl_node_kind::always); // Non spatialized,
                                  // relevant to all
    AddInfo(
        ALevelScriptActor::StaticClass(),
        Erepl_node_kind::none); // Not needed
#if 0                               // WITH_GAMEPLAY_DEBUGGER
    AddInfo(
        AGameplayDebuggerCategoryReplicator::StaticClass(),
        Erepl_node_kind::connection); // Only owner connection
                                                        // viable
#endif

    for (FClassReplicationPolicyPreset PolicyBP : ReplicationPolicySettings) {
        if (PolicyBP.Class) {
            AddInfo(PolicyBP.Class, PolicyBP.Policy);
        }
    }

    // this does contains all replicated class except GetIsReplicated is false
    // actor if someone need to make replication work, mark it as replicated and
    // control it over replication graph
    TArray<UClass *> AllReplicatedClasses;

    // Iterate all class
    for (TObjectIterator<UClass> It; It; ++It) {
        UClass * Class = *It;
        AActor * ActorCDO = Cast<AActor>(Class->GetDefaultObject());

        if (!ActorCDO || !ActorCDO->GetIsReplicated()) {
            continue;
        }

        // Skip SKEL and REINST classes.
        if (Class->GetName().StartsWith(TEXT("SKEL_")) ||
            Class->GetName().StartsWith(TEXT("REINST_"))) {
            continue;
        }

        // --------------------------------------------------------------------
        // This is a replicated class. Save this off for the second pass below
        // --------------------------------------------------------------------

        AllReplicatedClasses.Add(Class);

        // Skip if already in the map (added explicitly)
        if (ClassRepNodePolicies.Contains(Class, false)) {
            continue;
        }

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
        UClass * SuperClass = Class->GetSuperClass();
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
            AddInfo(Class, Erepl_node_kind::dynamic_spatial);
        } else if (
            ActorCDO->bAlwaysRelevant && !ActorCDO->bOnlyRelevantToOwner) {
            AddInfo(Class, Erepl_node_kind::always);
        } else if (ActorCDO->bOnlyRelevantToOwner) {
            AddInfo(Class, Erepl_node_kind::connection);
        }

        // TODO:: currently missing feature, !bAlwaysRelevant &&
        // bOnlyRelevantToOwner -> only owner see this but is spatialized
    }

    TArray<FClassReplicationInfoPreset> ValidClassReplicationInfoPreset;
    // custom setting
    for (FClassReplicationInfoPreset & ReplicationInfoBP :
         ReplicationInfoSettings) {
        if (ReplicationInfoBP.Class) {
            GlobalActorReplicationInfoMap.SetClassInfo(
                ReplicationInfoBP.Class,
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
                        return ReplicatedClass->IsChildOf(Info.Class.Get());
                    })) {
            // duplicated or set included child will be ignored
            if (Preset->Class.Get() == ReplicatedClass ||
                Preset->IncludeChildClasses) {
                continue;
            }
        }

        const bool bClassIsSpatialized =
            IsSpatialized(ClassRepNodePolicies.GetChecked(ReplicatedClass));

        FClassReplicationInfo ClassInfo;
        InitClassReplicationInfo(
            ClassInfo,
            ReplicatedClass,
            bClassIsSpatialized,
            NetDriver->NetServerMaxTickRate);
        GlobalActorReplicationInfoMap.SetClassInfo(ReplicatedClass, ClassInfo);
    }


    // Print out what we came up with
    UE_LOG(LogLocusReplicationGraph, Log, TEXT(""));
    UE_LOG(LogLocusReplicationGraph, Log, TEXT("Class Routing Map: "));
    UEnum * Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("Erepl_node_kind"));
    for (auto ClassMapIt = ClassRepNodePolicies.CreateIterator(); ClassMapIt;
         ++ClassMapIt) {
        UClass * Class =
            CastChecked<UClass>(ClassMapIt.Key().ResolveObjectPtr());
        const Erepl_node_kind Mapping = ClassMapIt.Value();

        // Only print if different than native class
        UClass * ParentNativeClass = GetParentNativeClass(Class);
        const Erepl_node_kind * ParentMapping =
            ClassRepNodePolicies.Get(ParentNativeClass);
        if (ParentMapping && Class != ParentNativeClass &&
            Mapping == *ParentMapping) {
            continue;
        }
        UE_LOG(
            LogLocusReplicationGraph,
            Log,
            TEXT("  %s (%s) -> %s"),
            *Class->GetName(),
            *GetNameSafe(ParentNativeClass),
            *Enum->GetNameStringByValue(static_cast<uint32>(Mapping)));
    }

    UE_LOG(LogLocusReplicationGraph, Log, TEXT(""));
    UE_LOG(LogLocusReplicationGraph, Log, TEXT("Class Settings Map: "));
    FClassReplicationInfo LocusValues;
    for (auto ClassRepInfoIt =
             GlobalActorReplicationInfoMap.CreateClassMapIterator();
         ClassRepInfoIt;
         ++ClassRepInfoIt) {
        const UClass * Class =
            CastChecked<UClass>(ClassRepInfoIt.Key().ResolveObjectPtr());
        const FClassReplicationInfo & ClassInfo = ClassRepInfoIt.Value();
        UE_LOG(
            LogLocusReplicationGraph,
            Log,
            TEXT("  %s (%s) -> %s"),
            *Class->GetName(),
            *GetNameSafe(GetParentNativeClass(Class)),
            *ClassInfo.BuildDebugStringDelta());
    }


    // Rep destruct infos based on CVar value
    DestructInfoMaxDistanceSquared =
        DestructionInfoMaxDistance * DestructionInfoMaxDistance;

#if 0 // WITH_GAMEPLAY_DEBUGGER
    AGameplayDebuggerCategoryReplicator::NotifyDebuggerOwnerChange.AddUObject(
        this, &ULocusReplicationGraph::OnGameplayDebuggerOwnerChange);
#endif
}

void ULocusReplicationGraph::InitGlobalGraphNodes()
{
    // Preallocate some replication lists. comment the following 4 lines in case
    // you use UE5+
    // TODO: Remove. PreAllocateRepList(3, 12);
    // TODO: Remove. PreAllocateRepList(6, 12);
    // TODO: Remove. PreAllocateRepList(128, 64);
    // TODO: Remove. PreAllocateRepList(512, 16);

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

void ULocusReplicationGraph::InitConnectionGraphNodes(
    UNetReplicationGraphConnection * RepGraphConnection)
{
    Super::InitConnectionGraphNodes(RepGraphConnection);

    // UE_LOG(LogLocusReplicationGraph, Warning, TEXT("InitConnection : %s"),
    // *RepGraphConnection->NetConnection->PlayerController->GetName());
    ULocusReplicationConnectionGraph * locus_conn_mgr =
        Cast<ULocusReplicationConnectionGraph>(RepGraphConnection);
    if (!locus_conn_mgr) {
        UE_LOG(
            LogLocusReplicationGraph,
            Warning,
            TEXT("Unrecognized ConnectionDriver class, Expected "
                 "ULocusReplicationConnectionGraph"));
    }

    locus_conn_mgr->AlwaysRelevantForConnectionNode =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForConnection>();
    AddConnectionGraphNode(
        locus_conn_mgr->AlwaysRelevantForConnectionNode, RepGraphConnection);

    locus_conn_mgr->TeamConnectionNode =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForTeam>();
    AddConnectionGraphNode(
        locus_conn_mgr->TeamConnectionNode, RepGraphConnection);

    // don't care about team names as it's initial value is always no_team
}

void ULocusReplicationGraph::OnRemoveConnectionGraphNodes(
    UNetReplicationGraphConnection * RepGraphConnection)
{
    ULocusReplicationConnectionGraph * locus_conn_mgr =
        Cast<ULocusReplicationConnectionGraph>(RepGraphConnection);
    if (locus_conn_mgr) {
        if (locus_conn_mgr->team != no_team) {
            TeamConnectionListMap.RemoveConnectionFromTeam(
                locus_conn_mgr->team, locus_conn_mgr);
        }
    }
}

void ULocusReplicationGraph::RemoveClientConnection(
    UNetConnection * NetConnection)
{
    // we completely override super function

    int32 ConnectionId = 0;
    bool bFound = false;

    // Remove the RepGraphConnection associated with this NetConnection. Also
    // update ConnectionIds to stay compact.
    auto UpdateList = [&](TArray<UNetReplicationGraphConnection *> List) {
        for (int32 idx = 0; idx < Connections.Num(); ++idx) {
            UNetReplicationGraphConnection * ConnectionManager =
                Connections[idx];
            repCheck(ConnectionManager);

            if (ConnectionManager->NetConnection == NetConnection) {
                ensure(!bFound);
                // Nofity this to handle something - remove from team list
                OnRemoveConnectionGraphNodes(ConnectionManager);
                Connections.RemoveAtSwap(idx, 1, false);
                bFound = true;
            } else {
                ConnectionManager->ConnectionOrderNum = ConnectionId++;
            }
        }
    };

    UpdateList(Connections);
    UpdateList(PendingConnections);

    if (!bFound) {
        UE_LOG(
            LogLocusReplicationGraph,
            Warning,
            TEXT("UReplicationGraph::RemoveClientConnection could not find "
                 "connection in Connection (%d) or PendingConnections (%d) "
                 "lists"),
            Connections.Num(),
            PendingConnections.Num());
    }
}

void ULocusReplicationGraph::RouteAddNetworkActorToNodes(
    const FNewReplicatedActorInfo & ActorInfo,
    FGlobalActorReplicationInfo & GlobalInfo)
{
    Erepl_node_kind Policy = GetMappingPolicy(ActorInfo.Class);
    switch (Policy) {
    case Erepl_node_kind::none: {
        break;
    }

    case Erepl_node_kind::always: {
        AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
        break;
    }

    case Erepl_node_kind::connection:
    case Erepl_node_kind::team: {
        RouteAddNetworkActorToConnectionNodes(Policy, ActorInfo, GlobalInfo);
        break;
    }

    case Erepl_node_kind::static_spatial: {
        GridNode->AddActor_Static(ActorInfo, GlobalInfo);
        break;
    }

    case Erepl_node_kind::dynamic_spatial: {
        GridNode->AddActor_Dynamic(ActorInfo, GlobalInfo);
        break;
    }

    case Erepl_node_kind::dormant_spatial: {
        GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);
        break;
    }
    };
}

void ULocusReplicationGraph::RouteRemoveNetworkActorToNodes(
    const FNewReplicatedActorInfo & ActorInfo)
{
    Erepl_node_kind Policy = GetMappingPolicy(ActorInfo.Class);

    switch (Policy) {
    case Erepl_node_kind::none: {
        break;
    }

    case Erepl_node_kind::always: {
        AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
        break;
    }

    case Erepl_node_kind::connection:
    case Erepl_node_kind::team: {
        RouteRemoveNetworkActorToConnectionNodes(Policy, ActorInfo);
        break;
    }

    case Erepl_node_kind::static_spatial: {
        GridNode->RemoveActor_Static(ActorInfo);
        break;
    }

    case Erepl_node_kind::dynamic_spatial: {
        GridNode->RemoveActor_Dynamic(ActorInfo);
        break;
    }

    case Erepl_node_kind::dormant_spatial: {
        GridNode->RemoveActor_Dormancy(ActorInfo);
        break;
    }
    };
}

// this function will be called seamless map transition
// as all actors will be removed in silly order, we have to deal with it
void ULocusReplicationGraph::ResetGameWorldState()
{
    Super::ResetGameWorldState();

    // all actor will be destroyed. just reset it.
    PendingConnectionActors.clear();
    PendingTeamRequests.clear();
#pragma warning(push)
#pragma warning(disable : 4458)
    auto EmptyConnectionNode =
        [](TArray<UNetReplicationGraphConnection *> & Connections) {
            for (UNetReplicationGraphConnection * conn_mgr : Connections) {
                if (ULocusReplicationConnectionGraph * locus_conn_mgr =
                        Cast<ULocusReplicationConnectionGraph>(conn_mgr)) {
                    locus_conn_mgr->AlwaysRelevantForConnectionNode
                        ->NotifyResetAllNetworkActors();
                }
            }
        };
#pragma warning(pop)
    EmptyConnectionNode(PendingConnections);
    EmptyConnectionNode(Connections);

    // as connection does not destroyed, we keep it
    // TeamConnectionListMap.Reset();
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

void ULocusReplicationGraph::AddDependentActor(
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
                LogLocusReplicationGraph,
                Warning,
                TEXT("ReplicatorActor privided is not replicating"));
        }
    }
}

void ULocusReplicationGraph::RemoveDependentActor(
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
                LogLocusReplicationGraph,
                Warning,
                TEXT("ReplicatorActor privided is not replicating"));
        }
    }
}

void ULocusReplicationGraph::ChangeOwnerOfAnActor(
    AActor * ActorToChange, AActor * NewOwner)
{
    Erepl_node_kind Policy = GetMappingPolicy(ActorToChange->GetClass());
    if (!ActorToChange || Policy == Erepl_node_kind::none ||
        IsSpatialized(Policy)) {
        // Policy doesn't matter for chaning owner
        return;
    }

    // remove from previous connection specific nodes.
    RouteRemoveNetworkActorToConnectionNodes(
        Policy, FNewReplicatedActorInfo(ActorToChange));

    // change owner safely
    ActorToChange->SetOwner(NewOwner);

    // re-route to connection specific nodes with new owner
    FGlobalActorReplicationInfo & GlobalInfo =
        GlobalActorReplicationInfoMap.Get(ActorToChange);
    RouteAddNetworkActorToConnectionNodes(
        Policy, FNewReplicatedActorInfo(ActorToChange), GlobalInfo);
}

void ULocusReplicationGraph::SetTeamForPlayerController(
    APlayerController * PlayerController, int NextTeam)
{
    if (PlayerController) {
        if (ULocusReplicationConnectionGraph * conn_mgr =
                FindLocusConnectionGraph(PlayerController)) {
            int CurrentTeam = conn_mgr->team;
            if (CurrentTeam != NextTeam) {
                if (CurrentTeam != no_team) {
                    TeamConnectionListMap.RemoveConnectionFromTeam(
                        CurrentTeam, conn_mgr);
                }

                if (NextTeam != no_team) {
                    TeamConnectionListMap.AddConnectionToTeam(
                        NextTeam, conn_mgr);
                }
                conn_mgr->team = NextTeam;
            }
        } else {
            PendingTeamRequests.push_back(
                team_request{NextTeam, PlayerController});
        }
    }
}

void ULocusReplicationGraph::RouteAddNetworkActorToConnectionNodes(
    Erepl_node_kind Policy,
    const FNewReplicatedActorInfo & ActorInfo,
    FGlobalActorReplicationInfo & GlobalInfo)
{
    if (ULocusReplicationConnectionGraph * conn_mgr =
            FindLocusConnectionGraph(ActorInfo.GetActor())) {
        switch (Policy) {
        case Erepl_node_kind::connection: {
            conn_mgr->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(
                ActorInfo);
            break;
        }
        case Erepl_node_kind::team: {
            conn_mgr->TeamConnectionNode->NotifyAddNetworkActor(ActorInfo);
            break;
        }
        };
    } else if (ActorInfo.Actor->GetNetOwner()) {
        // this actor is not yet ready. add to pending array to handle pending
        // route
        PendingConnectionActors.push_back(ActorInfo.GetActor());
    }
}


void ULocusReplicationGraph::RouteRemoveNetworkActorToConnectionNodes(
    Erepl_node_kind Policy, const FNewReplicatedActorInfo & ActorInfo)
{
    if (ULocusReplicationConnectionGraph * conn_mgr =
            FindLocusConnectionGraph(ActorInfo.GetActor())) {
        switch (Policy) {
        case Erepl_node_kind::connection: {
            conn_mgr->AlwaysRelevantForConnectionNode
                ->NotifyRemoveNetworkActor(ActorInfo);
            break;
        }
        case Erepl_node_kind::team: {
            conn_mgr->TeamConnectionNode->NotifyRemoveNetworkActor(
                ActorInfo);
            break;
        }
        };
    } else if (ActorInfo.Actor->GetNetOwner()) {
        // this actor is not yet ready. but doesn't matter the pending array
        // contains the actor or not
        std::erase(PendingConnectionActors, ActorInfo.GetActor());
    }
}

void ULocusReplicationGraph::HandlePendingActorsAndTeamRequests()
{
    if (!PendingTeamRequests.empty()) {
        std::vector TempRequests = std::move(PendingTeamRequests);

        for (auto & Request : TempRequests) {
            if (Request.pc && Request.pc->IsValidLowLevel()) {
                // if failed, it will automatically re-added to pending list
                SetTeamForPlayerController(Request.pc, Request.team);
            }
        }
    }

    if (!PendingConnectionActors.empty()) {
        std::vector TempActors = std::move(PendingConnectionActors);

        for (AActor * Actor : TempActors) {
            if (Actor && Actor->IsValidLowLevel()) {
                if (UNetConnection * Connection = Actor->GetNetConnection()) {
                    // if failed, it will automatically re-added to pending list
                    Erepl_node_kind Policy =
                        GetMappingPolicy(Actor->GetClass());
                    FGlobalActorReplicationInfo & GlobalInfo =
                        GlobalActorReplicationInfoMap.Get(Actor);
                    RouteAddNetworkActorToConnectionNodes(
                        Policy, FNewReplicatedActorInfo(Actor), GlobalInfo);
                }
            }
        }
    }
}


class ULocusReplicationConnectionGraph *
ULocusReplicationGraph::FindLocusConnectionGraph(const AActor * Actor)
{
    if (Actor) {
        if (UNetConnection * NetConnection = Actor->GetNetConnection()) {
            if (ULocusReplicationConnectionGraph * conn_mgr =
                    Cast<ULocusReplicationConnectionGraph>(
                        FindOrAddConnectionManager(NetConnection))) {
                return conn_mgr;
            }
        }
    }
    return nullptr;
}

#if 0 // WITH_GAMEPLAY_DEBUGGER
void ULocusReplicationGraph::OnGameplayDebuggerOwnerChange(
    AGameplayDebuggerCategoryReplicator * Debugger,
    APlayerController * OldOwner)
{
    if (ULocusReplicationConnectionGraph * conn_mgr =
            FindLocusConnectionGraph(OldOwner)) {
        FNewReplicatedActorInfo ActorInfo(Debugger);
        conn_mgr->AlwaysRelevantForConnectionNode->NotifyRemoveNetworkActor(
            ActorInfo);
    }

    if (ULocusReplicationConnectionGraph * conn_mgr =
            FindLocusConnectionGraph(Debugger->GetReplicationOwner())) {
        FNewReplicatedActorInfo ActorInfo(Debugger);
        conn_mgr->AlwaysRelevantForConnectionNode->NotifyAddNetworkActor(
            ActorInfo);
    }
}
#endif

void ULocusReplicationGraph::PrintRepNodePolicies()
{
    UEnum * Enum = FindObject<UEnum>(ANY_PACKAGE, TEXT("Erepl_node_kind"));
    if (!Enum) {
        return;
    }

    GLog->Logf(TEXT("===================================="));
    GLog->Logf(TEXT("Shooter Replication Routing Policies"));
    GLog->Logf(TEXT("===================================="));

    for (auto It = ClassRepNodePolicies.CreateIterator(); It; ++It) {
        FObjectKey ObjKey = It.Key();

        Erepl_node_kind Mapping = It.Value();

        GLog->Logf(
            TEXT("%-40s --> %s"),
            *GetNameSafe(ObjKey.ResolveObjectPtr()),
            *Enum->GetNameStringByValue(static_cast<uint32>(Mapping)));
    }
}

Erepl_node_kind ULocusReplicationGraph::GetMappingPolicy(UClass * Class)
{
    Erepl_node_kind * PolicyPtr = ClassRepNodePolicies.Get(Class);
    Erepl_node_kind Policy = PolicyPtr ? *PolicyPtr : Erepl_node_kind::none;
    return Policy;
}

void UReplicationGraphNode_AlwaysRelevant_ForTeam::
    GatherActorListsForConnection(
        const FConnectionGatherActorListParameters & Params)
{
    ULocusReplicationConnectionGraph * locus_conn_mgr =
        Cast<ULocusReplicationConnectionGraph>(&Params.ConnectionManager);
    if (locus_conn_mgr && locus_conn_mgr->team != no_team) {
        ULocusReplicationGraph * ReplicationGraph =
            Cast<ULocusReplicationGraph>(GetOuter());
        if (TArray<ULocusReplicationConnectionGraph *> * TeamConnections =
                ReplicationGraph->TeamConnectionListMap
                    .GetConnectionArrayForTeam(locus_conn_mgr->team)) {
            for (ULocusReplicationConnectionGraph * TeamMember :
                 *TeamConnections) {
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
    ULocusReplicationGraph * ReplicationGraph =
        Cast<ULocusReplicationGraph>(GetOuter());
    ReplicationGraph->HandlePendingActorsAndTeamRequests();
}

void UReplicationGraphNode_AlwaysRelevant_ForTeam::
    GatherActorListsForConnectionDefault(
        const FConnectionGatherActorListParameters & Params)
{
    Super::GatherActorListsForConnection(Params);
}

TArray<class ULocusReplicationConnectionGraph *> *
FTeamConnectionListMap::GetConnectionArrayForTeam(int team)
{
    return Find(team);
}

void FTeamConnectionListMap::AddConnectionToTeam(
    int team, ULocusReplicationConnectionGraph * conn_mgr)
{
    TArray<class ULocusReplicationConnectionGraph *> & TeamList =
        FindOrAdd(team);
    TeamList.Add(conn_mgr);
}

void FTeamConnectionListMap::RemoveConnectionFromTeam(
    int team, ULocusReplicationConnectionGraph * conn_mgr)
{
    if (TArray<class ULocusReplicationConnectionGraph *> * TeamList =
            Find(team)) {
        TeamList->RemoveSwap(conn_mgr);
        // remove team if there's noone left
        if (TeamList->Num() == 0) {
            Remove(team);
        }
    }
}


// console commands copied from shooter repgraph
//  ------------------------------------------------------------------------------

FAutoConsoleCommandWithWorldAndArgs ShooterPrintRepNodePoliciesCmd(
    TEXT("LocusRepGraph.PrintRouting"),
    TEXT("Prints how actor classes are routed to RepGraph nodes"),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString> & Args, UWorld * World) {
            for (TObjectIterator<ULocusReplicationGraph> It; It; ++It) {
                It->PrintRepNodePolicies();
            }
        }));


FAutoConsoleCommandWithWorldAndArgs ChangeFrequencyBucketsCmd(
    TEXT("LocusRepGraph.FrequencyBuckets"),
    TEXT("Resets frequency bucket count."),
    FConsoleCommandWithWorldAndArgsDelegate::CreateLambda(
        [](const TArray<FString> & Args, UWorld * World) {
            int32 Buckets = 1;
            if (Args.Num() > 0) {
                LexTryParseString<int32>(Buckets, *Args[0]);
            }

            UE_LOG(
                LogLocusReplicationGraph,
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
