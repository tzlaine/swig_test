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

#include "Agame_mode.h"
#include "Amap_fleet.h"
#include "Amap_hex.h"
#include "Amap_system.h"
#include "Aplayer_controller.h"
#include "config.hpp"
#include "model.hpp"

#include <Engine/LevelScriptActor.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <UObject/UObjectIterator.h>


namespace {
    void remove_connection_from_teams(
        std::map<int, std::vector<Urepl_graph_conn *>> & map,
        Urepl_graph_conn * conn)
    {
        for (auto & [_, conns] : map) {
            std::erase(conns, conn);
        }
        std::erase_if(map, [](auto const & e) { return e.second.empty(); });
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
            PUSH_DISABLE_WARNING(4996)
            info.SetCullDistanceSquared(cdo->NetCullDistanceSquared);
            POP_WARNING
            UE_LOG(
                LogReplicationGraph,
                Log,
                TEXT("Setting cull distance for %s to %f (%f)"),
                *class_->GetName(),
                info.GetCullDistanceSquared(),
                FMath::Sqrt(info.GetCullDistanceSquared()));
        }

        PUSH_DISABLE_WARNING(4996)
        info.ReplicationPeriodFrame = std::max(
            (uint32)FMath::RoundToFloat(
                server_max_tick_rate / cdo->NetUpdateFrequency),
            1u);
        POP_WARNING

        UClass * native_class = class_;
        while (!native_class->IsNative() && native_class->GetSuperClass() &&
               native_class->GetSuperClass() != AActor::StaticClass()) {
            native_class = native_class->GetSuperClass();
        }

        PUSH_DISABLE_WARNING(4996)
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("Setting replication period for %s (%s) to %d frames (%.2f)"),
            *class_->GetName(),
            *native_class->GetName(),
            info.ReplicationPeriodFrame,
            cdo->NetUpdateFrequency);
        POP_WARNING
    }

    UClass const * parent_native_class_of(UClass const * class_)
    {
        while (class_ && !class_->IsNative()) {
            class_ = class_->GetSuperClass();
        }
        return class_;
    }

    bool spatial(Erepl_node_kind k)
    {
        return Erepl_node_kind::static_spatial <= k;
    }
}

Urepl_graph_with_pending_node::Urepl_graph_with_pending_node()
{
    bRequiresPrepareForReplicationCall = true;
}

void Urepl_graph_with_pending_node::PrepareForReplication()
{
    Cast<Urepl_graph>(GetOuter())->process_pendings();
}

void Urepl_graph_team_node::GatherActorListsForConnection(
    FConnectionGatherActorListParameters const & params)
{
    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(&params.ConnectionManager);
    if (conn && conn->nation_id != nation_none) {
        Urepl_graph * repl_graph = Cast<Urepl_graph>(GetOuter());
        auto const it = repl_graph->team_to_conn_.find(conn->nation_id);
        if (it != repl_graph->team_to_conn_.end()) {
            for (Urepl_graph_conn * team_conn : it->second) {
                team_conn->team_node->gather_impl(params);
            }
        }
    } else {
        Super::GatherActorListsForConnection(params);
    }
}

void Urepl_graph_team_node::gather_impl(
    FConnectionGatherActorListParameters const & params)
{
    Super::GatherActorListsForConnection(params);
}

Urepl_graph::Urepl_graph() : UReplicationGraph()
{
    UE_LOG(LogReplicationGraph, Log, TEXT("Urepl_graph constructed"));

    ReplicationConnectionManagerClass = Urepl_graph_conn::StaticClass();

    explicit_classes_.emplace_back(
        Amap_hex::StaticClass(), class_repl_info{}, Erepl_node_kind::team);

    explicit_classes_.emplace_back(
        Amap_system::StaticClass(), class_repl_info{}, Erepl_node_kind::team);

    explicit_classes_.emplace_back(
        Amap_fleet::StaticClass(),
        class_repl_info{},
        Erepl_node_kind::static_spatial);

    check(std::ranges::none_of(
        explicit_classes_, [](auto const & e) { return !e.class_; }));
}

Urepl_graph::~Urepl_graph()
{
    auto * w = GetWorld();
    if (!w)
        return;
    if (auto * gm = Cast<Agame_mode>(w->GetAuthGameMode()))
        gm->notify_repl_graph_destroyed(this);
}

void Urepl_graph::InitGlobalActorClassSettings()
{
    Super::InitGlobalActorClassSettings();

    check(GetWorld());
    if (auto * gm = Cast<Agame_mode>(GetWorld()->GetAuthGameMode()))
        gm->notify_repl_graph_constructed(this);

    auto add_routing = [&](UClass * class_, Erepl_node_kind routing) {
        class_to_routing_.Set(class_, routing);
    };

    add_routing(
        AReplicationGraphDebugActor::StaticClass(), Erepl_node_kind::none);
    add_routing(AInfo::StaticClass(), Erepl_node_kind::always);
    add_routing(ALevelScriptActor::StaticClass(), Erepl_node_kind::none);

    for (auto const & [class_, repl_info, routing] : explicit_classes_) {
        GlobalActorReplicationInfoMap.SetClassInfo(class_, repl_info);
        add_routing(class_, routing);
    }

    TArray<UClass *> all_repl_classes;

    for (TObjectIterator<UClass> it; it; ++it) {
        UClass * class_ = *it;
        AActor * actor_cdo = Cast<AActor>(class_->GetDefaultObject());

        if (!actor_cdo || !actor_cdo->GetIsReplicated())
            continue;

        if (class_->GetName().StartsWith(TEXT("SKEL_")) ||
            class_->GetName().StartsWith(TEXT("REINST_"))) {
            continue;
        }

        all_repl_classes.Add(class_);

        if (class_to_routing_.Contains(class_, false))
            continue;

        UClass * super_class = class_->GetSuperClass();
        if (AActor * super_cdo =
                Cast<AActor>(super_class->GetDefaultObject())) {
            if (super_cdo->GetIsReplicated() == actor_cdo->GetIsReplicated() &&
                super_cdo->bAlwaysRelevant == actor_cdo->bAlwaysRelevant &&
                super_cdo->bOnlyRelevantToOwner ==
                    actor_cdo->bOnlyRelevantToOwner &&
                super_cdo->bNetUseOwnerRelevancy ==
                    actor_cdo->bNetUseOwnerRelevancy) {
                continue;
            }
        }

        if (actor_cdo->GetIsReplicated() && !actor_cdo->bAlwaysRelevant &&
            !actor_cdo->bOnlyRelevantToOwner &&
            !actor_cdo->bNetUseOwnerRelevancy) {
            add_routing(class_, Erepl_node_kind::dynamic_spatial);
        } else if (
            actor_cdo->bAlwaysRelevant && !actor_cdo->bOnlyRelevantToOwner) {
            add_routing(class_, Erepl_node_kind::always);
        } else if (actor_cdo->bOnlyRelevantToOwner) {
            add_routing(class_, Erepl_node_kind::connection);
        }
    }

    UReplicationGraphNode_ActorListFrequencyBuckets::DefaultSettings.ListSize =
        12;

    for (UClass * repl_class : all_repl_classes) {
        auto const it = std::ranges::find_if(
            explicit_classes_, [repl_class](auto const & e) {
                return repl_class->IsChildOf(e.class_.Get());
            });
        if (it != explicit_classes_.end()) {
            if (it->class_.Get() == repl_class ||
                it->repl_info_.include_child_classes)
                continue;
        }

        FClassReplicationInfo class_info;
        populate_class_repl_info(
            class_info,
            repl_class,
            spatial(class_to_routing_.GetChecked(repl_class)),
            // clang-format off
        PUSH_DISABLE_WARNING(4996)
        NetDriver->NetServerMaxTickRate);
        // clang-format on
        POP_WARNING
        GlobalActorReplicationInfoMap.SetClassInfo(repl_class, class_info);
    }

    UE_LOG(LogReplicationGraph, Log, TEXT(""));
    UE_LOG(LogReplicationGraph, Log, TEXT("Class Routing Map: "));
    UEnum const * Enum = StaticEnum<Erepl_node_kind>();
    for (auto it = class_to_routing_.CreateIterator(); it; ++it) {
        UClass * class_ = CastChecked<UClass>(it.Key().ResolveObjectPtr());
        Erepl_node_kind const routing = it.Value();

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
            *Enum->GetNameStringByValue((uint32)routing));
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
}

void Urepl_graph::InitGlobalGraphNodes()
{
    always_node = CreateNewNode<Urepl_graph_with_pending_node>();
    AddGlobalGraphNode(always_node);
}

void Urepl_graph::InitConnectionGraphNodes(
    UNetReplicationGraphConnection * repl_graph_conn)
{
    Super::InitConnectionGraphNodes(repl_graph_conn);

    Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(repl_graph_conn);
    if (!conn) {
        UE_LOG(
            LogReplicationGraph,
            Warning,
            TEXT("Unrecognized ConnectionDriver class, Expected "
                 "Urepl_graph_conn"));
        return;
    }

    conn->always_node =
        CreateNewNode<UReplicationGraphNode_AlwaysRelevant_ForConnection>();
    AddConnectionGraphNode(conn->always_node, repl_graph_conn);

    conn->team_node = CreateNewNode<Urepl_graph_team_node>();
    AddConnectionGraphNode(conn->team_node, repl_graph_conn);
}

void Urepl_graph::RemoveClientConnection(UNetConnection * net_conn)
{
    int32 conn_id = 0;
    bool found = false;

    auto update = [&](TArray<UNetReplicationGraphConnection *> list) {
        for (int32 idx = 0; idx < list.Num(); ++idx) {
            UNetReplicationGraphConnection * conn_base = list[idx];
            check(conn_base);

            if (conn_base->NetConnection == net_conn) {
                ensure(!found);
                if (Urepl_graph_conn * conn =
                        Cast<Urepl_graph_conn>(conn_base)) {
                    remove_connection_from_teams(team_to_conn_, conn);
                }
                list.RemoveAtSwap(idx, 1, EAllowShrinking::No);
                found = true;
            } else {
                conn_base->ConnectionOrderNum = conn_id++;
            }
        }
    };

    update(Connections);
    update(PendingConnections);

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
    insert_actor(actor_info.GetActor());
}

void Urepl_graph::RouteRemoveNetworkActorToNodes(
    FNewReplicatedActorInfo const & actor_info)
{
    erase_actor(actor_info.GetActor());
}

void Urepl_graph::ResetGameWorldState()
{
    Super::ResetGameWorldState();

    pending_actor_reqs_.clear();
    pending_team_reqs_.clear();
    PUSH_DISABLE_WARNING(4458)
    auto clear_node = [](TArray<UNetReplicationGraphConnection *> & conns) {
        for (UNetReplicationGraphConnection * conn_base : conns) {
            if (Urepl_graph_conn * conn = Cast<Urepl_graph_conn>(conn_base))
                conn->always_node->NotifyResetAllNetworkActors();
        }
    };
    POP_WARNING
    clear_node(PendingConnections);
    clear_node(Connections);
}

void Urepl_graph::use_model(std::shared_ptr<model> const & m) { model_ = m; }

void Urepl_graph::team_insert(Aplayer_controller * pc, int nation_id)
{
    if (!pc)
        return;
    if (Urepl_graph_conn * conn = find_connection_graph(pc)) {
        if (nation_id == nation_none) // adding pc to its own "team"
            nation_id = conn->nation_id = pc->nation_id();
        team_to_conn_[nation_id].push_back(conn);
    } else {
        pending_team_reqs_.push_back(team_request{pc, nation_id, false});
    }
}

void Urepl_graph::team_erase(Aplayer_controller * pc, int nation_id)
{
    check(nation_id != nation_none);
    if (!pc)
        return;
    if (Urepl_graph_conn * conn = find_connection_graph(pc)) {
        auto it = team_to_conn_.find(nation_id);
        if (it == team_to_conn_.end())
            return;
        std::erase(it->second, conn);
        std::erase_if(
            team_to_conn_, [](auto const & e) { return e.second.empty(); });
    } else {
        pending_team_reqs_.push_back(team_request{pc, nation_id, true});
    }
}

void Urepl_graph::insert_actor(AActor * a)
{
    insert_erase_actor_impl(
        a,
        [](auto * node, auto const & actor_info) {
            node->NotifyAddNetworkActor(actor_info);
        },
        false);
}

void Urepl_graph::erase_actor(AActor * a)
{
    insert_erase_actor_impl(
        a,
        [](auto * node, auto const & actor_info) {
            node->NotifyRemoveNetworkActor(actor_info);
        },
        true);
}

void Urepl_graph::reinsert_actor(AActor * a)
{
    erase_actor(a);
    insert_actor(a);
}

void Urepl_graph::print_rep_node_kinds()
{
    UEnum const * enum_ = StaticEnum<Erepl_node_kind>();
    if (!enum_)
        return;

    UE_LOG(LogReplicationGraph, Log, TEXT("Replication Graph Routing:"));
    for (auto it = class_to_routing_.CreateIterator(); it; ++it) {
        FObjectKey key = it.Key();
        Erepl_node_kind routing = it.Value();
        UE_LOG(
            LogReplicationGraph,
            Log,
            TEXT("%-40s --> %s"),
            *GetNameSafe(key.ResolveObjectPtr()),
            *enum_->GetNameStringByValue((uint32)routing));
    }
}

void Urepl_graph::process_pendings()
{
    std::vector tmp_reqs = std::move(pending_team_reqs_);
    for (auto req : tmp_reqs) {
        if (req.pc && req.pc->IsValidLowLevel()) {
            if (req.erase)
                team_erase(req.pc, req.nation_id);
            else
                team_insert(req.pc, req.nation_id);
        }
    }

    std::vector tmp_actors = std::move(pending_actor_reqs_);
    for (auto req : tmp_actors) {
        if (!req.a || !req.a->IsValidLowLevel())
            continue;
        UNetConnection * conn = req.a->GetNetConnection();
        if (!conn)
            continue;
        if (req.erase)
            erase_actor(req.a);
        else
            insert_actor(req.a);
    }
}

Urepl_graph_conn *
Urepl_graph::find_connection_graph(Aplayer_controller const * pc)
{
    if (!pc)
        return nullptr;

    if (UNetConnection * net_conn = pc->GetNetConnection()) {
        if (Urepl_graph_conn * conn =
                Cast<Urepl_graph_conn>(FindOrAddConnectionManager(net_conn))) {
            return conn;
        }
    }

    return nullptr;
}

Erepl_node_kind Urepl_graph::routing_for(AActor * a)
{
    auto * const ptr = class_to_routing_.Get(a->GetClass());
    return ptr ? *ptr : Erepl_node_kind::none;
}
