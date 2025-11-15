#pragma once

#include "map_util.hpp"

#include <algorithm>
#include <span>

#include <CoreMinimal.h>
#include <ReplicationGraph.h>
#include "Uvisibility_repl_graph_node.generated.h"


class Aplayer_controller;

UCLASS()
class Uvisibility_repl_graph_node : public UReplicationGraphNode
{
    GENERATED_BODY()

public:
    Uvisibility_repl_graph_node();

    void map_and_players(
        FBox2D extents_box,
        std::span<Aplayer_controller *> players,
        double max_detection_radius_before_stealth);

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

    void add_to_cells(AActor * a, Aplayer_controller const * owner);

    void set_allied(int nation_id_1, int nation_id_2, bool value);

    FIntPoint cell_pos(AActor * a) const
    {
        auto const loc = a->GetActorLocation() - bias_;
        int const x = int(loc.X / cell_size);
        int const y = int(loc.Y / cell_size);
        assert(0 <= x && x < width_);
        assert(0 <= y && y < height_);
        return FIntPoint(x, y);
    }

    // Positions of elements here correspond to he top-level indices used in
    // actors_ and cells_.
    std::vector<Aplayer_controller const *> players_;
    
    std::vector<uint8> allied_; // size() is players_.size() * players_.size()

    struct actors
    {
        std::vector<AActor *> actors_;
        std::vector<AActor *> removed_actors_;
    };

    std::vector<actors> all_actors_;

    std::vector<FIntPoint> max_radius_offsets_;

    struct cell
    {
        // TODO: Fleet actors specifically?  Maybe one vec of fleets, and
        // another of systems?  Systems can spot fleets, but not vice
        // versa....
        std::vector<AActor *> actors_;
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

    inline static double const cell_size = 4.0;
};
