#pragma once

#include "Amap_pawn_base.h"
#include "map_util.hpp"
#include "proximity_grid.hpp"

#include <algorithm>
#include <span>

#include <CoreMinimal.h>
#include <ReplicationGraph.h>
#include "Uvisibility_repl_graph_node.generated.h"


class Aplayer_controller;
struct game_state_t;

UCLASS()
class Uvisibility_repl_graph_node : public UReplicationGraphNode
{
    GENERATED_BODY()

public:
    Uvisibility_repl_graph_node();

    void new_game(game_state_t const & gs);

    void update_detection_range(game_state_t const & gs);

    void NotifyAddNetworkActor(FNewReplicatedActorInfo const & info) override;

    bool NotifyRemoveNetworkActor(
        FNewReplicatedActorInfo const & info,
        bool warn_if_not_found = true) override;

    bool NotifyActorRenamed(
        FRenamedReplicatedActorInfo const & info,
        bool warn_if_not_found = true) override;

    void GatherActorListsForConnection(
        FConnectionGatherActorListParameters const & params) override;

    void PrepareForReplication() override;

private:
    std::vector<std::vector<Amap_pawn_base *>> all_nations_actors_;
    proximity_grid<Amap_pawn_base> proximity_grid_;
};
