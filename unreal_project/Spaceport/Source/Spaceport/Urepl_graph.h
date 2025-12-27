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

#include "Amap_hex.h"
#include "Amap_system.h"
#include "constants.hpp"
#include "model.hpp"
#include "model_visibility.hpp"

#include <map>
#include <vector>

#include <CoreMinimal.h>
#include <ReplicationGraph.h>
#include "Urepl_graph.generated.h"


class Aplayer_controller;
class Urepl_graph_conn;
struct model;

// The use of "team" in this file is somewhat misleading.  The "team" is just
// the ID of a single nation; there are no actual teams as such.  Each player
// that is allied with a nation with ID N gets added to team N.  The actors
// visible to a given player are the actors they own (fleets) or can see
// (everything else) directly -- since those are in their nation/"team", plus
// the actors in the various teams/nations with whom they are allied.  Outside
// of this, the rules for spatially-relevant visibility apply.

UENUM()
enum class Erepl_node_kind : uint8 {
    none,

    always,
    connection,
    team,

    static_spatial, // TODO
    dynamic_spatial,
    dormant_spatial,
};

UCLASS()
class Urepl_graph_with_pending_node : public UReplicationGraphNode_ActorList
{
    GENERATED_BODY()

public:
    Urepl_graph_with_pending_node();
    void PrepareForReplication() override;
};

UCLASS()
class Urepl_graph_team_node : public UReplicationGraphNode_ActorList
{
    GENERATED_BODY()

public:
    void GatherActorListsForConnection(
        FConnectionGatherActorListParameters const & params) override;

private:
    void gather_impl(FConnectionGatherActorListParameters const & params);
};

UCLASS()
class Urepl_graph_conn : public UNetReplicationGraphConnection
{
    GENERATED_BODY()

public:
    UPROPERTY()
    UReplicationGraphNode_AlwaysRelevant_ForConnection * always_node;

    UPROPERTY()
    Urepl_graph_team_node * team_node;

    // aka team
    int nation_id = nation_none;
};

UCLASS()
class Urepl_graph : public UReplicationGraph
{
    GENERATED_BODY()

public:
    Urepl_graph();
    ~Urepl_graph();

    void InitGlobalActorClassSettings() override;
    void InitGlobalGraphNodes() override;
    void InitConnectionGraphNodes(
        UNetReplicationGraphConnection * repl_graph_conn) override;
    void RemoveClientConnection(UNetConnection * net_conn) override;
    void RouteAddNetworkActorToNodes(
        FNewReplicatedActorInfo const & actor_info,
        FGlobalActorReplicationInfo & global_info) override;
    void RouteRemoveNetworkActorToNodes(
        FNewReplicatedActorInfo const & actor_info) override;
    void ResetGameWorldState() override;

    void use_model(std::shared_ptr<model> const & m);

    void team_insert(Aplayer_controller * pc, int nation_id = nation_none);
    void team_erase(Aplayer_controller * pc, int nation_id);

    void insert_actor(AActor * a);
    void erase_actor(AActor * a);
    void reinsert_actor(AActor * a);

    void print_rep_node_kinds();

private:
    struct team_request
    {
        Aplayer_controller * pc = nullptr;
        int nation_id = nation_none;
        bool erase = false;
    };
    struct actor_request
    {
        AActor * a = nullptr;
        bool erase = false;
    };

    // TODO: FClassReplicationInfo necessary?
    struct class_repl_info : FClassReplicationInfo
    {
        bool include_child_classes = true;
    };

    struct explicit_class
    {
        TSubclassOf<AActor> class_;
        class_repl_info repl_info_;
        Erepl_node_kind routing = Erepl_node_kind::none;
    };

    void process_pendings();
    Urepl_graph_conn * find_connection_graph(Aplayer_controller const * pc);
    Erepl_node_kind routing_for(AActor * a);

    template<typename F>
    void insert_erase_actor_impl(AActor * a, F && f, bool erase)
    {
        check(a);
        // TODO: Other enumerators....
        auto const routing = routing_for(a);
        switch (routing) {
        case Erepl_node_kind::none: break;
        case Erepl_node_kind::always:
            f(always_node, FNewReplicatedActorInfo(a));
            break;
        case Erepl_node_kind::connection: {
            auto * pc = Cast<Aplayer_controller>(a);
            check(pc);
            if (Urepl_graph_conn * conn = find_connection_graph(pc))
                f(conn->always_node, FNewReplicatedActorInfo(a));
            else
                pending_actor_reqs_.push_back({a, erase});
            break;
        }
        case Erepl_node_kind::team: {
            if (!PendingConnections.IsEmpty()) {
                pending_actor_reqs_.push_back({a, erase});
                break;
            }

            std::shared_ptr<model> m = model_.lock();
            if (!m) {
                UE_LOG(
                    LogReplicationGraph,
                    Error,
                    TEXT("Unable to route actor '%s' of type %s (unable "
                         "get access to model)."),
                    *a->GetName(),
                    *a->GetClass()->GetName());
                break;
            }

            auto gs = m->game_state();
            check(gs);

            for (auto * c : Connections) {
                auto * conn = Cast<Urepl_graph_conn>(c);
                check(conn);
                check(conn->nation_id != nation_none);
                int const nation_id = conn->nation_id;
                visibility_kind vis = visibility_kind::unseen;
                if (auto * hex = Cast<Amap_hex>(a)) {
                    if (hex->id() == hex_none) {
                        pending_actor_reqs_.push_back({a, erase});
                        break;
                    }
                    vis = visibility_of(
                        *gs, {}, nation_id, gs->hexes[hex->id()], 0);
                } else if (auto * system = Cast<Amap_system>(a)) {
                    if (system->id() == system_none) {
                        pending_actor_reqs_.push_back({a, erase});
                        break;
                    }
                    vis = visibility_of(
                        *gs,
                        {},
                        nation_id,
                        gs->systems[system->id()],
                        system->id());
                }
                if (vis != visibility_kind::unseen)
                    f(conn->team_node, FNewReplicatedActorInfo(a));
            }
            break;
        }
        }
    }

    UPROPERTY()
    Urepl_graph_with_pending_node * always_node;

    TClassMap<Erepl_node_kind> class_to_routing_;
    std::map<int, std::vector<Urepl_graph_conn *>> team_to_conn_;
    std::vector<actor_request> pending_actor_reqs_;
    std::vector<team_request> pending_team_reqs_;
    std::vector<explicit_class> explicit_classes_;
    std::weak_ptr<model> model_;

    friend Urepl_graph_team_node;
    friend Urepl_graph_with_pending_node;
};

inline const bool repl_graph_in_use = [] {
    UReplicationDriver::CreateReplicationDriverDelegate().BindLambda(
        [](UNetDriver *, FURL const &, UWorld *) -> UReplicationDriver * {
            return NewObject<Urepl_graph>(GetTransientPackage());
        });
    return true;
}();
