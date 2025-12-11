#include "Aplayer_controller.h"
#include "Agame_mode.h"
#include "Amap_fleet.h"
#include "Amap_system.h"
#include "Amap_hex.h"
#include "Aplaying_hud.h"
#include "utility.hpp"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Engine/LocalPlayer.h>
#include <Engine/World.h>


namespace {
    Aplaying_hud * cast(AHUD * base) { return Cast<Aplaying_hud>(base); }
}

Aplayer_controller::Aplayer_controller() {}

void Aplayer_controller::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplayer_controller::BeginPlay()"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplayer_controller::BeginPlay()"));
}

void Aplayer_controller::Tick(float delta)
{
    Super::Tick(delta);

    if (selection_box_first_ != selection_box_last_)
        return;

    FHitResult hit_result;
    if (GetHitResultUnderCursor(fleet_channel, false, hit_result)) {
        Amap_fleet * fleet = Cast<Amap_fleet>(hit_result.GetActor());
        if (fleet)
            hover(fleet);
    } else if (GetHitResultUnderCursor(star_channel, false, hit_result)) {
        Amap_system * system = Cast<Amap_system>(hit_result.GetActor());
        if (system)
            hover(system);
    } else if (GetHitResultUnderCursor(hex_channel, false, hit_result)) {
        Amap_hex * hex = Cast<Amap_hex>(hit_result.GetActor());
        if (hex)
            hover(hex);
    } else {
        dehover_all();
    }
}

void Aplayer_controller::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent * eic =
        Cast<UEnhancedInputComponent>(InputComponent);
    check(eic);

    if (!select_object_action_ || !order_selected_action_ ||
        !pause_toggle_action_ || !keep_selected_action_ ||
        !alternate_selection_action_) {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Player controller does not have all its actions assigned"));
        return;
    }

    eic->BindActionValueLambda(
        select_object_action_, ETriggerEvent::Started, [this](auto const &) {
            auto * hud = cast(GetHUD());
            check(hud);
            GetMousePosition(selection_box_first_.X, selection_box_first_.Y);
            hud->set_selection_box_first(selection_box_first_);
        });
    eic->BindActionValueLambda(
        select_object_action_, ETriggerEvent::Triggered, [this](auto const &) {
            auto * hud = cast(GetHUD());
            check(hud);
            GetMousePosition(selection_box_last_.X, selection_box_last_.Y);
            hud->set_selection_box_last(selection_box_last_);
            if (selection_box_first_ != selection_box_last_)
                dehover_all();
        });
    eic->BindActionValueLambda(
        select_object_action_, ETriggerEvent::Completed, [this](auto const &) {
            if (selection_box_first_ != selection_box_last_) {
                select_in_box(
                    alternate_selection_key_down_ ? map_pawn_kind::system
                                                  : map_pawn_kind::fleet,
                    keep_selected_key_down_ ? deselect::no : deselect::yes);
                selection_box_first_ = selection_box_last_ = FVector2D();
                auto * hud = cast(GetHUD());
                check(hud);
                hud->set_selection_box_first(FVector2D());
                hud->set_selection_box_last(FVector2D());
                return;
            }

            FHitResult hit_result;
            if (GetHitResultUnderCursor(fleet_channel, false, hit_result)) {
                Amap_fleet * fleet = Cast<Amap_fleet>(hit_result.GetActor());
                if (fleet) {
                    select(
                        fleet,
                        keep_selected_key_down_ ? deselect::no : deselect::yes);
                }
            } else if (GetHitResultUnderCursor(
                           star_channel, false, hit_result)) {
                Amap_system * system = Cast<Amap_system>(hit_result.GetActor());
                if (system) {
                    select(
                        system,
                        keep_selected_key_down_ ? deselect::no : deselect::yes);
                }
            } else if (GetHitResultUnderCursor(
                           hex_channel, false, hit_result)) {
                Amap_hex * hex = Cast<Amap_hex>(hit_result.GetActor());
                if (hex) {
                    select(
                        hex,
                        keep_selected_key_down_ ? deselect::no : deselect::yes);
                }
            } else {
                deselect_all();
            }
        });

    eic->BindActionValueLambda(
        order_selected_action_, ETriggerEvent::Completed, [this](auto const &) {
            // TODO
        });

    eic->BindActionValueLambda(
        pause_toggle_action_, ETriggerEvent::Completed, [this](auto const &) {
            // TODO
        });

    eic->BindActionValueLambda(
        keep_selected_action_, ETriggerEvent::Started, [this](auto const &) {
            keep_selected_key_down_ = true;
        });
    eic->BindActionValueLambda(
        keep_selected_action_, ETriggerEvent::Completed, [this](auto const &) {
            keep_selected_key_down_ = false;
        });

    eic->BindActionValueLambda(
        alternate_selection_action_,
        ETriggerEvent::Started,
        [this](auto const &) { alternate_selection_key_down_ = true; });
    eic->BindActionValueLambda(
        alternate_selection_action_,
        ETriggerEvent::Completed,
        [this](auto const &) { alternate_selection_key_down_ = false; });
}

void Aplayer_controller::server_quit_to_menu_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->multicast_quit_to_menu();
}

void Aplayer_controller::server_start_game_Implementation(
    TArray<uint8> const & params)
{
    if (auto * hud = cast(GetHUD()))
        hud->remove_game_setup_widget();

    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->setup_for_game_start(params);
}

void Aplayer_controller::server_save_game_Implementation(
    FString const & filename)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->save_game(filename);
}

void Aplayer_controller::send_initial_game_state_to_client(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_initial_game_state(state);
}

void Aplayer_controller::client_recv_initial_game_state_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::send_day_updates_to_client(TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_day_updates(state);
}

void Aplayer_controller::client_recv_day_updates_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::send_month_updates_to_client(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_month_updates(state);
}

void Aplayer_controller::client_recv_month_updates_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::send_year_updates_to_client(TArray<uint8> const & state)
{
    if (HasAuthority())
        client_recv_year_updates(state);
}

void Aplayer_controller::client_recv_year_updates_Implementation(
    TArray<uint8> const & state)
{
    if (HasAuthority())
        return;
    // TODO
}

void Aplayer_controller::server_toggle_pause_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->toggle_pause();
}

void Aplayer_controller::server_change_play_speed_Implementation(int speed)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->play_speed(speed);
}

void Aplayer_controller::dehover_all()
{
    for (auto * p : curr_hovers_) {
        p->hover(false);
    }
    curr_hovers_.clear();
}

void Aplayer_controller::deselect_all()
{
    for (auto * p : curr_selections_) {
        p->select(false);
    }
    curr_selections_.clear();
}

void Aplayer_controller::hover(Amap_pawn_base * pawn)
{
    dehover_all();
    if (std::ranges::any_of(
            curr_selections_, [pawn](auto * e) { return e == pawn; })) {
        return;
    }
    pawn->hover(true);
    curr_hovers_.push_back(pawn);
}

void Aplayer_controller::select(Amap_pawn_base * pawn, deselect deselect_curr)
{
    map_pawn_kind const kind = pawn->kind();
    if (!curr_selections_.empty() && deselect_curr == deselect::no &&
        kind == curr_selections_[0]->kind()) {
        auto const it = std::ranges::lower_bound(curr_selections_, pawn);
        if (it != curr_selections_.end() && *it == pawn) {
            // Group selection is in effect; deselect pawn.
            pawn->select(false);
            curr_selections_.erase(it);
            return;
        }
    }

    Amap_pawn_base ** pawn_ptr = &pawn;
    select(std::span(pawn_ptr, pawn_ptr + 1), deselect_curr, kind);
}

void Aplayer_controller::select(
    std::span<Amap_pawn_base *> pawns,
    deselect deselect_curr,
    map_pawn_kind kind)
{
    // Only allow additional selections of the same kind of thing, but don't
    // allow multiselection of hexes.
    if (!curr_selections_.empty() && deselect_curr == deselect::no) {
        if (kind == map_pawn_kind::hex)
            return;
        if (kind != curr_selections_[0]->kind())
            return;
    }

    // TODO: Consider having this class contain multiple instanced geometry
    // components, instead of having hover, selection, etc., live in each
    // actor.

    FBox2D box;
    {
        FVector2D const points[2] = {selection_box_first_, selection_box_last_};
        box = FBox2D(points, 2);
    }

    if (deselect_curr == deselect::yes)
        deselect_all();
    for (auto * p : pawns) {
        if (p->kind() != kind)
            continue;
        FVector2D xy;
        ProjectWorldLocationToScreen(p->GetActorLocation(), xy);
        if (!box.IsInsideOrOn(xy))
            continue;
        p->select(true);
        curr_selections_.push_back(p);
    }
    std::ranges::sort(curr_selections_, std::ranges::less{});
    curr_selections_.resize(
        curr_selections_.size() - std::ranges::unique(curr_selections_).size());
}

void Aplayer_controller::select_in_box(
    map_pawn_kind selecting, deselect deselect_curr)
{
    auto * hud = cast(GetHUD());
    check(hud);
    TArray<Amap_pawn_base *> & pawns = hud->selected_in_box();
    select(
        std::span<Amap_pawn_base *>(begin(pawns), end(pawns)),
        deselect_curr,
        selecting);
}
