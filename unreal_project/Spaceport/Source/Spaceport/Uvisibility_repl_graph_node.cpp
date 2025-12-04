#include "Uvisibility_repl_graph_node.h"

#include "Agame_mode.h"
#include "Aplayer_controller.h"
#include "model.hpp"

#include <Kismet/GameplayStatics.h>


namespace {
    Aplayer_controller const * pc_cast(AActor const * a)
    {
        return Cast<Aplayer_controller>(a);
    }

    Amap_pawn_base * actor_cast(AActor * a)
    {
        return Cast<Amap_pawn_base>(a);
    }
    Amap_pawn_base const * actor_cast(AActor const * a)
    {
        return Cast<Amap_pawn_base>(a);
    }
}

Uvisibility_repl_graph_node::Uvisibility_repl_graph_node() {}

void Uvisibility_repl_graph_node::new_game(game_state_t const & gs)
{
    proximity_grid_ = proximity_grid<Amap_pawn_base>(gs);
    all_nations_actors_.resize(gs.nations.size());
}

void Uvisibility_repl_graph_node::update_detection_range(
    game_state_t const & gs)
{
    proximity_grid_.max_detection_radius(
        max_detection_radius_before_stealth(gs));
}

void Uvisibility_repl_graph_node::NotifyAddNetworkActor(
    FNewReplicatedActorInfo const & info)
{
    auto * a = actor_cast(info.Actor);
    if (!a)
        return;
    proximity_grid_.insert(a);
    all_nations_actors_[a->nation_id()].push_back(a);
}

bool Uvisibility_repl_graph_node::NotifyRemoveNetworkActor(
    FNewReplicatedActorInfo const & info, bool)
{
    auto * a = actor_cast(info.Actor);
    if (!a)
        return false;
    proximity_grid_.erase(a);
    std::erase(all_nations_actors_[a->nation_id()], a);
    return true;
}

bool Uvisibility_repl_graph_node::NotifyActorRenamed(
    FRenamedReplicatedActorInfo const & info, bool)
{
    return true;
}

void push_back(FActorRepListRefView & actors, Amap_pawn_base * a)
{
    actors.Add(a);
}

void Uvisibility_repl_graph_node::GatherActorListsForConnection(
    FConnectionGatherActorListParameters const & params)
{
    FActorRepListRefView gathered_actors;
    push_back(gathered_actors, (Amap_pawn_base *)nullptr);

    auto * gm = Cast<Agame_mode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!gm) {
        UE_LOG(general, Error, TEXT(
            "Unable to get a pointer to the game state from "
            "Uvisibility_repl_graph_node::GatherActorListsForConnection!"));
    }
    auto const gs = gm->game_state();

    std::vector<int> allies;

    for (auto const & viewer : params.Viewers) {
        auto * pc = pc_cast(viewer.InViewer);
        if (!pc)
            continue;

        proximity_grid_.gather_visible_objects(
            gathered_actors,
            pc->nation_id(),
            all_nations_actors_[pc->nation_id()],
            *gs,
            find_visible::non_allied);

        allies_of(allies, *gs, pc->nation_id());
        for (int ally : allies) {
            proximity_grid_.gather_visible_objects(
                gathered_actors,
                ally,
                all_nations_actors_[ally],
                *gs,
                find_visible::non_allied);
        }
    }

    if (!gathered_actors.IsEmpty()) {
        params.OutGatheredReplicationLists.AddReplicationActorList(
            gathered_actors);
    }
}

void Uvisibility_repl_graph_node::PrepareForReplication()
{
    proximity_grid_.prepare_to_gather_objects();
}
