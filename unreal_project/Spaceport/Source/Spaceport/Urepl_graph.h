#pragma once

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

#include <map>
#include <vector>

#include <CoreMinimal.h>
#include <ReplicationGraph.h>
#include "Urepl_graph.generated.h"


// class UReplicationGraphNode_GridSpatialization2D;
class AGameplayDebuggerCategoryReplicator;
class Urepl_graph_conn;

// TODO: Move.
inline constexpr int no_team = -1;

// This is the main enum we use to route actors to the right replication node.
// Each class maps to one enum.
UENUM(BlueprintType)
enum class Erepl_node_kind : uint8 {
    // Requires manual intervention.
    none,

    // Routes to an AlwaysRelevantNode
    always,
    // Routes to an AlwaysRelevantNode_ForConnection node
    connection,
    // Routes to an AlwaysRelevantNode_ForTeam node
    team,

    // ONLY SPATIALIZED Enums below here! See
    // Urepl_graph::IsSpatialized

    // Routes to GridNode: these actors don't move and don't need to be updated
    // every frame.
    static_spatial,
    // Routes to GridNode: these actors mode frequently and are updated once per
    // frame.
    dynamic_spatial,
    // Routes to GridNode: While dormant we treat as static. When flushed/not
    // dormant dynamic. Note this is for things that "move while not dormant".
    dormant_spatial,
};


USTRUCT(BlueprintType)
struct FClassReplicationPolicyPreset
{
    GENERATED_BODY()

public:
    // Class to set replication policy.
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> class_;
    // Policy to set.
    UPROPERTY(EditAnywhere)
    Erepl_node_kind routing;
};


USTRUCT(BlueprintType)
struct FClassReplicationInfoPreset
{
    GENERATED_BODY()

public:
    // Class of this Replication info is related
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> class_;
    // How much will distance affect to priority
    UPROPERTY(
        EditAnywhere,
        meta =
            (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float DistancePriorityScale = 1.f;
    // How much will stavation affect to priority
    UPROPERTY(
        EditAnywhere,
        meta =
            (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float StarvationPriorityScale = 1.f;
    // Cull distance that overrides NetCullDistance (Warning : IsNetRelevantFor
    // will not be called in this system)
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
    float CullDistanceSquared = 0.f;
    // Server frame count per actual replication
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
    uint8 ReplicationPeriodFrame = 1;
    // How long will this actor channel stay alive even after it's being out of
    // relevancy
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
    uint8 ActorChannelFrameTimeout = 4;
    // Whether this setting overrides all child classes or not
    UPROPERTY(EditAnywhere)
    bool IncludeChildClasses = true;

    FClassReplicationInfo CreateClassReplicationInfo()
    {
        FClassReplicationInfo retval;
        retval.DistancePriorityScale = DistancePriorityScale;
        retval.StarvationPriorityScale = StarvationPriorityScale;
        retval.SetCullDistanceSquared(CullDistanceSquared);
        retval.ReplicationPeriodFrame = ReplicationPeriodFrame;
        retval.ActorChannelFrameTimeout = ActorChannelFrameTimeout;
        return retval;
    }
};

UCLASS()
class UReplicationGraphNode_AlwaysRelevant_WithPending
    : public UReplicationGraphNode_ActorList
{
    GENERATED_BODY()

public:
    UReplicationGraphNode_AlwaysRelevant_WithPending();
    void PrepareForReplication() override;
};

UCLASS()
class UReplicationGraphNode_AlwaysRelevant_ForTeam
    : public UReplicationGraphNode_ActorList
{
    GENERATED_BODY()

public:
    // Gather up other team member's list
    void GatherActorListsForConnection(
        FConnectionGatherActorListParameters const & Params) override;

    // Function that calls parent ActorList's GatherActorList...
    virtual void GatherActorListsForConnectionDefault(
        FConnectionGatherActorListParameters const & Params);
};

// ReplicationConnectionGraph that holds team information and connection
// specific nodes.
UCLASS()
class Urepl_graph_conn : public UNetReplicationGraphConnection
{
    GENERATED_BODY()

public:
    UPROPERTY()
    UReplicationGraphNode_AlwaysRelevant_ForConnection *
        AlwaysRelevantForConnectionNode;

    UPROPERTY()
    class UReplicationGraphNode_AlwaysRelevant_ForTeam * TeamConnectionNode;

    int team = no_team;
};

UCLASS(Blueprintable)
class Urepl_graph : public UReplicationGraph
{
    GENERATED_BODY()

public:
    // How far destruction infos will be sent. When server destroyed an actor
    // that is NetLoadOnClient tick is on
    UPROPERTY(
        EditDefaultsOnly,
        meta =
            (ClampMin = "1000.0",
             ClampMax = "100000.0",
             UIMin = "1000.0",
             UIMax = "100000.0"))
    float DestructionInfoMaxDistance = 30000.f;

    // Cell size of spatial gird.
    UPROPERTY(
        EditDefaultsOnly,
        meta =
            (ClampMin = "1000.0",
             ClampMax = "100000.0",
             UIMin = "1000.0",
             UIMax = "100000.0"))
    float SpacialCellSize = 10000.f;

    // Spatial grid out of bound bias. Better not change this.
    UPROPERTY(EditDefaultsOnly)
    FVector2D SpatialBias = FVector2D(-150000.f, -200000.f);

    // Should spatial grid rebuilt upon detecting an actor that is out of bias?
    UPROPERTY(EditDefaultsOnly)
    bool EnableSpatialRebuilds = false;

    UPROPERTY(EditDefaultsOnly)
    TArray<FClassReplicationPolicyPreset> ReplicationPolicySettings;

    UPROPERTY(EditDefaultsOnly)
    TArray<FClassReplicationInfoPreset> ReplicationInfoSettings;

public:
    Urepl_graph();

    // Set up Enums for each uclass of entire actor
    void InitGlobalActorClassSettings() override;

    // initialize global node, like gridnode
    void InitGlobalGraphNodes() override;

    // initialize per connection node, like always relevant node
    void InitConnectionGraphNodes(
        UNetReplicationGraphConnection * RepGraphConnection) override;
    // deinitialize per connection node
    virtual void OnRemoveConnectionGraphNodes(
        UNetReplicationGraphConnection * RepGraphConnection);

    // override to make notification when a connection manager is removed
    void
    RemoveClientConnection(UNetConnection * NetConnection) override;

    // routng actor add/removal
    void RouteAddNetworkActorToNodes(
        FNewReplicatedActorInfo const & ActorInfo,
        FGlobalActorReplicationInfo & GlobalInfo) override;
    void RouteRemoveNetworkActorToNodes(
        FNewReplicatedActorInfo const & ActorInfo) override;

    void ResetGameWorldState() override;

    // gridnode for spatialization handling
    UPROPERTY()
    UReplicationGraphNode_GridSpatialization2D * GridNode;

    // always relevant for all connection
    UPROPERTY()
    UReplicationGraphNode_AlwaysRelevant_WithPending * AlwaysRelevantNode;

    // always relevant for all connection but in streaming level, so always
    // relevant to connection who loaded key level TMap<int,
    // FActorRepListRefView> AlwaysRelevantStreamingLevelActors; but this is
    // not needed as AlwaysRelevantNode already handle streaming level

    // Add Dependent Actor to ReplicationActor's Dep List, DependentActor will
    // relevant according to Replicator's relevancy
    void AddDependentActor(AActor * ReplicatorActor, AActor * DependentActor);
    void
    RemoveDependentActor(AActor * ReplicatorActor, AActor * DependentActor);

    // Change Owner of an actor that is relevant to connection specific
    void ChangeOwnerOfAnActor(AActor * ActorToChange, AActor * NewOwner);

    // SetTeam via Name
    void set_team_for(
        APlayerController * pc, int team);

    // to handle actors that has no connection at addnofity execution
    void RouteAddNetworkActorToConnectionNodes(
        Erepl_node_kind routing,
        FNewReplicatedActorInfo const & ActorInfo,
        FGlobalActorReplicationInfo & GlobalInfo);
    void RouteRemoveNetworkActorToConnectionNodes(
        Erepl_node_kind routing, FNewReplicatedActorInfo const & ActorInfo);

    // handle pending team requests and notifies
    void HandlePendingActorsAndTeamRequests();

    Urepl_graph_conn *
    FindConnectionGraph(AActor const * Actor);

    // Just copy-pasted from ShooterGame
#if 0 // WITH_GAMEPLAY_DEBUGGER
    void OnGameplayDebuggerOwnerChange(
        AGameplayDebuggerCategoryReplicator * Debugger,
        APlayerController * OldOwner);
#endif

    void PrintRepNodePolicies();

private:
    struct team_request
    {
        int team;
        APlayerController * pc;
    };

    Erepl_node_kind routing_for(UClass * class_);

    bool IsSpatialized(Erepl_node_kind k) const
    {
        return Erepl_node_kind::static_spatial <= k;
    }

    TClassMap<Erepl_node_kind> ClassRepNodePolicies;

    friend UReplicationGraphNode_AlwaysRelevant_ForTeam;

    std::map<int, std::vector<Urepl_graph_conn *>> team_to_conn_;
    std::vector<AActor *> pending_actors_;
    std::vector<team_request> pending_team_requests_;
};
