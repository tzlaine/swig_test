#include "Uvisibility_repl_graph_node.h"

#include "Agame_actor.h"
#include "Aplayer_controller.h"
#include "model.hpp"


namespace {
    Aplayer_controller const * pc_cast(AActor const * a)
    {
        return Cast<Aplayer_controller>(a);
    }

    Agame_actor * actor_cast(AActor * a) { return Cast<Agame_actor>(a); }
    Agame_actor const * actor_cast(AActor const * a)
    {
        return Cast<Agame_actor>(a);
    }

    double const temp_detection_radius_TODO = 2.0;
}

Uvisibility_repl_graph_node::Uvisibility_repl_graph_node() : cells_(1u) {}

void Uvisibility_repl_graph_node::new_game(
    game_state_t const & game_state, std::span<Aplayer_controller *> players)
{
    FBox2D extents_box;
    {
        auto const game_state_extents_box = world_map_extent(game_state);
        extents_box.Min = FVector2D{
            game_state_extents_box.min_.x, game_state_extents_box.min_.y};
        extents_box.Max = FVector2D{
            game_state_extents_box.max_.x, game_state_extents_box.max_.y};
    }

    bias_ = FVector(extents_box.Min.X, extents_box.Min.Y, 0);

    width_ = (int)std::ceil(extents_box.GetExtent().X / cell_size);
    height_ = (int)std::ceil(extents_box.GetExtent().Y / cell_size);

    players_.assign(players.begin(), players.end());

    // TODO: Is anything required by the UReplicationGraphNode API here?
    cells_.resize(players_.size() + 1);
    std::ranges::fill(cells_, std::vector<cell>(width_ * height_));

    // TODO: Fill this in with actual alliance data for each player.  Each
    // player's list of ally IDs should be sorted.
    allied_.resize(players_.size() + 1);

    all_actors_.resize(players_.size() + 1);

    update_detection_range(game_state);
}

void Uvisibility_repl_graph_node::update_detection_range(
    game_state_t const & game_state)
{
    double const new_max_detection_radius_before_stealth =
        max_detection_radius_before_stealth(game_state);

    if (std::abs(
            new_max_detection_radius_before_stealth -
            max_detection_radius_before_stealth_) < epsilon) {
        return;
    }

    max_detection_radius_before_stealth_ =
        new_max_detection_radius_before_stealth;

    int const max_cell_radius =
        int(max_detection_radius_before_stealth_ / cell_size) + 1;

    double const max_radius_sq = max_detection_radius_before_stealth_ *
                                 max_detection_radius_before_stealth_;

    FBox2D const center_cell_box{
        FVector2D{-cell_size / 2.0, -cell_size / 2.0},
        FVector2D{cell_size / 2.0, cell_size / 2.0}};

    max_radius_offsets_.clear();
    for (int i = -max_cell_radius; i <= max_cell_radius; ++i) {
        for (int j = -max_cell_radius; j <= max_cell_radius; ++j) {
            FBox2D const cell_box{
                FVector2D{(i - 0.5) * cell_size, (j - 0.5) * cell_size},
                FVector2D{(i + 0.5) * cell_size, (j + 0.5) * cell_size}};
            double const distances[4] = {
                cell_box.ComputeSquaredDistanceToPoint(center_cell_box.Min),
                cell_box.ComputeSquaredDistanceToPoint(center_cell_box.Max),
                cell_box.ComputeSquaredDistanceToPoint(
                    FVector2D{center_cell_box.Min.X, center_cell_box.Max.Y}),
                cell_box.ComputeSquaredDistanceToPoint(
                    FVector2D{center_cell_box.Max.X, center_cell_box.Min.Y})};
            double const nearest_sq = std::ranges::min(distances);
            if (nearest_sq < max_radius_sq)
                max_radius_offsets_.push_back(FIntPoint{i, j});
        }
    }

    // sort to optimize for cache locality
    std::ranges::sort(max_radius_offsets_, [](auto const & a, auto const & b) {
        if (a.Y < b.Y)
            return true;
        if (b.Y < a.Y)
            return true;
        return a.X < b.X;
    });
}

void Uvisibility_repl_graph_node::NotifyAddNetworkActor(
    FNewReplicatedActorInfo const & info)
{
    auto * pc = pc_cast(info.Actor->GetNetOwner());
    auto * a = actor_cast(info.Actor);
    if (!a)
        return;

    all_actors_[pc_to_index(pc)].actors_.push_back(a);
}

bool Uvisibility_repl_graph_node::NotifyRemoveNetworkActor(
    FNewReplicatedActorInfo const & info, bool)
{
    auto * pc = pc_cast(info.Actor->GetNetOwner());
    auto * a = actor_cast(info.Actor);
    if (!a)
        return false;

    all_actors_[pc_to_index(pc)].removed_actors_.push_back(a);

    return true;
}

bool Uvisibility_repl_graph_node::NotifyActorRenamed(
    FRenamedReplicatedActorInfo const & info, bool)
{
    return true;
}

void Uvisibility_repl_graph_node::GatherActorListsForConnection(
    FConnectionGatherActorListParameters const & params)
{
    FActorRepListRefView gathered_actors;

    auto const process_actor = [&gathered_actors,
                                this](auto a, int player_index) {
        auto & cell = cell_for(cell_pos(a), player_index);
        auto const a_pos_world = a->GetActorLocation();
        for (auto cell_a : cell.actors_) {
            auto const cell_a_pos_world = cell_a->GetActorLocation();
            // TODO: Implement real detection logic
            if (FVector::DistSquaredXY(a_pos_world, cell_a_pos_world) <
                temp_detection_radius_TODO) {
                gathered_actors.Add(cell_a);
            }
        }
    };

    for (auto const & viewer : params.Viewers) {
        auto * pc = pc_cast(viewer.InViewer);
        if (!pc)
            continue;

        auto const player_index = pc_to_index(pc);
        for (auto a : all_actors_[player_index].actors_) {
            process_actor(a, player_index);
        }

        for (int ally : allied_[player_index]) {
            auto actors = std::ranges::equal_range(
                all_actors_[players_.size()].actors_,
                ally,
                std::ranges::less{},
                [](auto a) { return a->nation_id(); });
            for (auto a : actors) {
                process_actor(a, player_index);
            }
        }
    }

    if (!gathered_actors.IsEmpty()) {
        params.OutGatheredReplicationLists.AddReplicationActorList(
            gathered_actors);
    }
}

void Uvisibility_repl_graph_node::PrepareForReplication()
{
    std::ranges::fill(cells_, std::vector<cell>(width_ * height_));

    // This assumes the size of removed_actors_ is much smaller than the size
    // of actors_.
    int i = 0;
    for (auto & [actors, removed] : all_actors_) {
        std::ranges::sort(removed);
        std::erase_if(actors, [&removed](auto e) {
            return std::ranges::binary_search(removed, e);
        });
        removed.clear();

        for (auto a : actors) {
            add_to_cells(a, players_[i]);
        }

        ++i;
    }

    // Sort the .actors_ of the "everything else bucket"
    // (all_actors_[players_.size()]) by owner nation_id, to make looking for
    // allied actors easier in GatherActorListsForConnection().
    std::ranges::sort(
        all_actors_[players_.size()].actors_, std::ranges::less{}, [](auto a) {
            return a->nation_id();
        });
}

void Uvisibility_repl_graph_node::add_to_cells(
    Agame_actor * a, Aplayer_controller const * owner)
{
    auto const actor_pos = cell_pos(a);
    int const owner_index = pc_to_index(owner);
    int const owner_nation = a->nation_id();
    for (int i = 0; i < (int)cells_.size(); ++i) {
        // The replication graph will handle cases of
        // "team"-visibility (whether self or allied).
        if (i == owner_index)
            continue;
        if (std::ranges::binary_search(allied_[i], owner_nation))
            continue;

        for (auto pos : max_radius_offsets_) {
            pos += actor_pos;
            if (0 <= pos.X && pos.X < width_ && 0 <= pos.Y && pos.Y < height_) {
                auto & cell = cell_for(pos, i);
                cell.actors_.push_back(a);
            }
        }
    }
}

void Uvisibility_repl_graph_node::set_allied(
    int nation_id_1, int nation_id_2, bool value)
{
    check(nation_id_1 != nation_id_2);

    auto const change_alliance_status =
        [value](auto & allies, int other_nation) {
            auto const it = std::ranges::lower_bound(allies, other_nation);
            if (it == allies.end()) {
                allies.push_back(other_nation);
                return;
            }

            if (*it == other_nation) {
                if (!value)
                    allies.erase(it);
            } else {
                if (value)
                    allies.insert(it, other_nation);
            }
        };

    auto const nation_1_it =
        std::ranges::find_if(players_, [nation_id_1](auto e) {
            return e->nation_id() == nation_id_1;
        });
    if (nation_1_it != players_.end()) {
        int const index = nation_1_it - players_.begin();
        auto & allies = allied_[index];
        change_alliance_status(allies, nation_id_2);
    }

    auto const nation_2_it =
        std::ranges::find_if(players_, [nation_id_2](auto e) {
            return e->nation_id() == nation_id_2;
        });
    if (nation_2_it != players_.end()) {
        int const index = nation_2_it - players_.begin();
        auto & allies = allied_[index];
        change_alliance_status(allies, nation_id_1);
    }
}
