#include "Uvisibility_repl_graph_node.h"

#include "Aplayer_controller.h"


namespace {
    Aplayer_controller const * cast(AActor const * a)
    {
        return Cast<Aplayer_controller>(a);
    }

    double const temp_detection_radius_TODO = 2.0;
}

Uvisibility_repl_graph_node::Uvisibility_repl_graph_node() : cells_(1u) {}

void Uvisibility_repl_graph_node::map_and_players(
    FBox2D extents_box,
    std::span<Aplayer_controller *> players,
    double max_detection_radius_before_stealth)
{
    bias_ = FVector(extents_box.Min.X, extents_box.Min.Y, 0);

    width_ = (int)std::ceil(extents_box.GetExtent().X / cell_size);
    height_ = (int)std::ceil(extents_box.GetExtent().Y / cell_size);

    players_.assign(players.begin(), players.end());

    // TODO: Is anything required by the UReplicationGraphNode API here?
    cells_.resize(players_.size());
    std::ranges::fill(cells_, std::vector<cell>(width_ * height_));

    // TODO: Fill this in with actual alliance data.
    allied_.resize(players_.size() * players_.size());

    all_actors_.resize(players_.size());

    // TODO: It might be useful to know the max for each nation in players_,
    // so that the max for all nations but nation N can be used to populate
    // nation N's grid contents.
    int const max_cell_radius =
        int(max_detection_radius_before_stealth / cell_size) + 1;

    double const max_radius_sq = max_detection_radius_before_stealth *
                                 max_detection_radius_before_stealth;

    FBox2D const center_cell_box{
        FVector2D{-cell_size / 2.0, -cell_size / 2.0},
        FVector2D{cell_size / 2.0, cell_size / 2.0}};

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
    auto * pc = cast(info.Actor->GetNetOwner());
    if (!pc)
        return;

    all_actors_[pc_to_index(pc)].actors_.push_back(info.Actor);
}

bool Uvisibility_repl_graph_node::NotifyRemoveNetworkActor(
    FNewReplicatedActorInfo const & info, bool)
{
    auto * pc = cast(info.Actor->GetNetOwner());
    if (!pc)
        return false;

    all_actors_[pc_to_index(pc)].removed_actors_.push_back(info.Actor);

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

    for (auto const & viewer : params.Viewers) {
        auto * pc = cast(viewer.InViewer);
        if (!pc)
            continue;

        auto const player_index = pc_to_index(pc);
        for (auto a : all_actors_[player_index].actors_) {
            auto & cell = cell_for(cell_pos(a), player_index);
            auto const a_pos_world = a->GetActorLocation();
            for (auto cell_a : cell.actors_) {
                auto const cell_a_pos_world = cell_a->GetActorLocation();
                if (FVector::DistSquaredXY(a_pos_world, cell_a_pos_world) <
                    temp_detection_radius_TODO) { // TODO: Implement real detection logic
                    gathered_actors.Add(cell_a);
                }
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
}

void Uvisibility_repl_graph_node::add_to_cells(
    AActor * a, Aplayer_controller const * owner)
{
    auto const actor_pos = cell_pos(a);
    int const owner_index = pc_to_index(owner);
    for (auto pos : max_radius_offsets_) {
        pos += actor_pos;
        if (0 <= pos.X && pos.X < width_ && 0 <= pos.Y && pos.Y < height_) {
            for (int i = 0; i < (int)players_.size(); ++i) {
                // The replication graph will handle cases of
                // "team"-visibility (whether self or allied).
                if (i == owner_index)
                    continue;
                if (allied_[i + owner_index * players_.size()] == 1u)
                    continue;
                auto & cell = cell_for(pos, i);
                cell.actors_.push_back(a);
            }
        }
    }
}

void Uvisibility_repl_graph_node::set_allied(
    int nation_id_1, int nation_id_2, bool value)
{
    assert(nation_id_1 != nation_id_2);

    auto const nation_1_it =
        std::ranges::find_if(players_, [nation_id_1](auto e) {
            return e->nation_id() == nation_id_1;
        });
    auto const nation_2_it =
        std::ranges::find_if(players_, [nation_id_2](auto e) {
            return e->nation_id() == nation_id_2;
        });

    assert(nation_1_it != players_.end());
    assert(nation_2_it != players_.end());

    int const index_1 = nation_1_it - players_.begin();
    int const index_2 = nation_2_it - players_.begin();

    allied_[index_1 + index_2 * players_.size()] = value;
    allied_[index_2 + index_1 * players_.size()] = value;
}
