#pragma once

#include "map_util.hpp"

#include <algorithm>
#include <span>

#include <CoreMinimal.h>
#include <ReplicationGraph.h>
#include "Uvisibility_repl_graph_node.generated.h"


class Aplayer_controller;
class Agame_actor;
struct game_state_t;

UCLASS()
class Uvisibility_repl_graph_node : public UReplicationGraphNode
{
    GENERATED_BODY()

public:
    Uvisibility_repl_graph_node();

    void new_game(
        game_state_t const & game_state,
        std::span<Aplayer_controller *> players);

    void update_detection_range(game_state_t const & game_state);

    void insert_alliance(int nation_id_1, int nation_id_2)
    {
        set_allied(nation_id_1, nation_id_2, true);
    }
    void erase_alliance(int nation_id_1, int nation_id_2)
    {
        set_allied(nation_id_1, nation_id_2, false);
    }

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
    int pc_to_index(Aplayer_controller const * pc) const
    {
        return std::ranges::find(players_, pc) - players_.begin();
    }

    void add_to_cells(Agame_actor * a, Aplayer_controller const * owner);

    void set_allied(int nation_id_1, int nation_id_2, bool value);

    FIntPoint cell_pos(AActor const * a) const
    {
        auto const loc = a->GetActorLocation() - bias_;
        int const x = int(loc.X / cell_size);
        int const y = int(loc.Y / cell_size);
        assert(0 <= x && x < width_);
        assert(0 <= y && y < height_);
        return FIntPoint(x, y);
    }

    // Positions of elements here correspond to the top-level indices used in
    // allied_, all_actors_, and cells_.  Those other vectors have
    // players_.size() + 1 elements, so failed pc_to_index() calls (which find
    // .end()) will have a valid index there.
    std::vector<Aplayer_controller const *> players_;

    std::vector<std::vector<int>> allied_;

    struct actors
    {
        std::vector<Agame_actor *> actors_;
        std::vector<Agame_actor *> removed_actors_;
    };

    // last element's .actors_ sorted by nation owner ID
    std::vector<actors> all_actors_;

    std::vector<FIntPoint> max_radius_offsets_;

    struct cell
    {
        // TODO: Fleet actors specifically?  Maybe one vec of fleets, and
        // another of systems?  Systems can spot fleets, but not vice
        // versa....
        std::vector<Agame_actor *> actors_;
    };

    cell const & cell_for(FIntPoint pos, int player_index) const
    {
        return cells_[player_index][pos.X + pos.Y * width_];
    }
    cell & cell_for(FIntPoint pos, int player_index)
    {
        return cells_[player_index][pos.X + pos.Y * width_];
    }

    std::vector<std::vector<cell>> cells_;
    int height_ = 1;
    int width_ = 1;

    // The coordinates for objects in some hexes may be negative, but the
    // cells are all in positive space.  Subtracting this bias from all actor
    // positions puts them all in positive space.
    FVector bias_;

    double max_detection_radius_before_stealth_ = 0.0;

    inline static double const cell_size = 4.0;
};
