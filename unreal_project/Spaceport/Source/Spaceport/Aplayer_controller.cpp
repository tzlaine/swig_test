#include "Aplayer_controller.h"
#include "Acontroller_pawn.h"
#include "Agame_mode.h"
#include "Agame_state.h"
#include "Ahud_t.h"
#include "Amap_fleet.h"
#include "Amap_hex.h"
#include "Amap_system.h"
#include "Aplayer_state.h"
#include "audio_assets.h"
#include "game_user_settings.h"
#include "game_data_formatters.hpp"
#include "map_transition.hpp"
#include "materials.h"
#include "rng.hpp"
#include "space_creator_actor_config.hpp"
#include "textures.h"
#include "ui_defaults.h"
#include "utility.hpp"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Materials/MaterialInterface.h>
#include <Engine/LocalPlayer.h>
#include <Engine/World.h>
#include <UObject/ConstructorHelpers.h>


namespace {
    Ahud_t * cast(AHUD * base) { return Cast<Ahud_t>(base); }

    Aplayer_state * cast(APlayerState * base)
    {
        return Cast<Aplayer_state>(base);
    }

    bool dragging(FVector2D first, FVector2D last)
    {
        float const min_drag_distance = 5.0f;
        float const min_dist_sq = min_drag_distance * min_drag_distance;
        return min_dist_sq < (first - last).SquaredLength();
    }
}

Aplayer_controller::Aplayer_controller()
{
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = true;
    map_transition_ = std::make_shared<map_transition_state>();
}

void Aplayer_controller::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameAndUI input_mode;
    input_mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    input_mode.SetHideCursorDuringCapture(false);
    SetInputMode(input_mode);
    SetShowMouseCursor(true);
    SetIgnoreLookInput(true);

    check(input_mapping_ctx_);

    if (ULocalPlayer * local_player = GetLocalPlayer()) {
        if (auto * input_sys =
                local_player
                    ->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
            if (UEnhancedInputUserSettings * user_settings =
                    input_sys->GetUserSettings()) {
                if (!user_settings->IsMappingContextRegistered(
                        input_mapping_ctx_))
                    user_settings->RegisterInputMappingContext(
                        input_mapping_ctx_);
            }
            if (!input_sys->HasMappingContext(input_mapping_ctx_)) {
                FModifyContextOptions options = {};
                options.bNotifyUserSettings = true;
                input_sys->AddMappingContext(input_mapping_ctx_, 0, options);
            }
        }
    }

    // KLUDGE: Since this player controller is unavailable at the time
    // Ugame_user_settings::LoadSettings() gets called, it cannot apply the
    // audio settings yet.  Now that this player controller is within its
    // lifetime, let's give the Ugame_user_settings a chance to apply any
    // deferred audio settings.
    Ugame_user_settings::get()->apply_volume_settings();
}

void Aplayer_controller::SetupInputComponent()
{
    Super::SetupInputComponent();

    EnableInput(this);

    auto * eic = Cast<UEnhancedInputComponent>(InputComponent);
    check(eic);

    if (!menu_toggle_action_ || !select_object_action_ ||
        !order_selected_action_ || !incr_play_speed_action_ ||
        !decr_play_speed_action_ || !pause_toggle_action_ ||
        !keep_selected_action_ || !alternate_selection_action_) {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Player controller does not have all its actions assigned"));
        return;
    }

    // always in use
    eic->BindActionValueLambda(
        menu_toggle_action_, ETriggerEvent::Completed, [this](auto const &) {
            if (auto * hud = ::hud(GetHUD()))
                hud->escape_pressed();
        });

    auto const use_map_actions = [this] {
        if (map_transition_->in_transition())
            return false;
        auto * gs = GetWorld()->GetGameState<Agame_state>();
        check(gs);
        return gs->playing_or_paused();
    };

    auto const end_drag = [this] {
        selection_box_first_ = selection_box_last_ = FVector2D();
        auto * hud = cast(GetHUD());
        check(hud);
        hud->set_selection_box_first(FVector2D());
        hud->set_selection_box_last(FVector2D());
    };

    auto const movement_speedup = [this] {
        return keep_selected_key_down_ ? map_move_modififier_factor : 1.0f;
    };

    eic->BindActionValueLambda(
        slide_action_,
        ETriggerEvent::Triggered,
        [use_map_actions, movement_speedup, this](auto const & value) {
            if (!use_map_actions())
                return;

            auto * camera_pawn = Cast<Acontroller_pawn>(GetPawn());
            check(camera_pawn);

            // TODO: Put the map size in Agame_state, and use that here to
            // bound how far we can move in any direction.

            FVector2D const delta = value.Get<FVector2D>() *
                                    ui_defaults().camera_pan_speed_ *
                                    movement_speedup();
            camera_pawn->AddMovementInput(FVector::UnitX(), delta.X);
            camera_pawn->AddMovementInput(FVector::UnitY(), delta.Y);
        });

    eic->BindActionValueLambda(
        zoom_action_,
        ETriggerEvent::Triggered,
        [use_map_actions, movement_speedup, this](auto const & value) {
            if (!use_map_actions())
                return;

            auto * camera_pawn = Cast<Acontroller_pawn>(GetPawn());
            check(camera_pawn);

            float const delta = value.Get<float>() *
                                (ui_defaults().camera_zoom_speed_ +
                                 std::log(camera_pawn->target_arm_length())) *
                                movement_speedup();
            float const desired_arm_length =
                camera_pawn->target_arm_length() + delta;

            // pushing down when already at the bottom of the galaxy map takes
            // us to the system map, if we're already pointing at a system
            if (map_transition_->mode() == map_mode::galaxy_map &&
                camera_pawn->target_arm_length() <
                    min_camera_dist_for(map_mode::galaxy_map) + 0.1 &&
                curr_hovers_.size() == 1u) {
                if (auto * map_system =
                        Cast<Amap_system>(curr_hovers_.front())) {
                    double_select(curr_hovers_.front());
                }
            }

            camera_pawn->target_arm_length(std::clamp(
                desired_arm_length,
                min_camera_dist_for(map_transition_->mode()),
                max_camera_dist_for(map_transition_->mode())));

            // pulling up when already at the top of the system map takes us
            // to the galaxy map
            if (map_transition_->mode() == map_mode::system_map &&
                -just_inside_system_map < camera_pawn->target_arm_length()) {
                camera_pawn->target_arm_length(-just_inside_system_map);
                map_transition_->to_galaxy_map(camera_pawn->camera_location());
            }
        });

    eic->BindActionValueLambda(
        select_object_action_,
        ETriggerEvent::Started,
        [use_map_actions, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu())
                return;
            auto * hud = cast(GetHUD());
            check(hud);
            GetMousePosition(selection_box_first_.X, selection_box_first_.Y);
            hud->set_selection_box_first(selection_box_first_);
        });
    eic->BindActionValueLambda(
        select_object_action_,
        ETriggerEvent::Triggered,
        [use_map_actions, end_drag, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu()) {
                end_drag();
                return;
            }
            auto * hud = cast(GetHUD());
            check(hud);
            GetMousePosition(selection_box_last_.X, selection_box_last_.Y);
            hud->set_selection_box_last(selection_box_last_);
            if (dragging(selection_box_first_, selection_box_last_))
                dehover_all();
        });
    eic->BindActionValueLambda(
        select_object_action_,
        ETriggerEvent::Completed,
        [use_map_actions, end_drag, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu()) {
                end_drag();
                return;
            }

            if (dragging(selection_box_first_, selection_box_last_)) {
                select_in_box(
                    alternate_selection_key_down_ ? map_pawn_kind::system
                                                  : map_pawn_kind::fleet,
                    keep_selected_key_down_ ? deselect::no : deselect::yes);
                end_drag();
                return;
            }

            end_drag();

            auto double_selected = [&](auto * target) {
                auto now = std::chrono::system_clock::now();
                if (!keep_selected_key_down_ && prev_select_target_ == target &&
                    (now - prev_select_time_) <
                        std::chrono::duration<float>(
                            max_double_click_interval_s)) {
                    double_select(target);
                    prev_select_target_ = nullptr;
                    prev_select_time_ =
                        std::chrono::time_point<std::chrono::system_clock>();
                    return true;
                }
                prev_select_target_ = target;
                prev_select_time_ = now;
                return false;
            };

            FHitResult hit_result;
            if (GetHitResultUnderCursor(fleet_channel, false, hit_result)) {
                Amap_fleet * fleet = Cast<Amap_fleet>(hit_result.GetActor());
                if (fleet) {
                    if (double_selected(fleet))
                        return;
                    select(
                        fleet,
                        keep_selected_key_down_ ? deselect::no : deselect::yes);
                }
            } else if (GetHitResultUnderCursor(
                           star_channel, false, hit_result)) {
                Amap_system * system = Cast<Amap_system>(hit_result.GetActor());
                if (system) {
                    if (double_selected(system))
                        return;
                    select(
                        system,
                        keep_selected_key_down_ ? deselect::no : deselect::yes);
                }
            } else if (GetHitResultUnderCursor(
                           hex_channel, false, hit_result)) {
                Amap_hex * hex = Cast<Amap_hex>(hit_result.GetActor());
                if (hex) {
                    if (double_selected(hex))
                        return;
                    select(
                        hex,
                        keep_selected_key_down_ ? deselect::no : deselect::yes);
                }
            } else {
                deselect_all();
            }
        });

    eic->BindActionValueLambda(
        order_selected_action_,
        ETriggerEvent::Completed,
        [use_map_actions, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu())
                return;
            // TODO
        });

    eic->BindActionValueLambda(
        incr_play_speed_action_,
        ETriggerEvent::Completed,
        [use_map_actions, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu())
                return;
            auto * gs = GetWorld()->GetGameState<Agame_state>();
            check(gs);
            if (gs->play_speed_ < 5)
                server_change_play_speed(gs->play_speed_ + 1);
        });

    eic->BindActionValueLambda(
        decr_play_speed_action_,
        ETriggerEvent::Completed,
        [use_map_actions, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu())
                return;
            auto * gs = GetWorld()->GetGameState<Agame_state>();
            check(gs);
            if (1 < gs->play_speed_)
                server_change_play_speed(gs->play_speed_ - 1);
        });

    eic->BindActionValueLambda(
        pause_toggle_action_,
        ETriggerEvent::Completed,
        [use_map_actions, this](auto const &) {
            if (!use_map_actions())
                return;
            if (showing_main_menu())
                return;
            server_toggle_pause();
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

void Aplayer_controller::Tick(float delta)
{
    Super::Tick(delta);

    map_transition_->tick(delta);
    if (auto new_camera_location = map_transition_->new_camera_location()) {
        auto * camera_pawn = Cast<Acontroller_pawn>(GetPawn());
        check(camera_pawn);
        if (new_camera_location)
            camera_pawn->camera_location(*new_camera_location);
    }
    if (auto new_star_location = map_transition_->new_star_location()) {
        check(system_star_);
        system_star_->SetActorLocation(*new_star_location);
    }

    if (showing_main_menu())
        return;

    if (dragging(selection_box_first_, selection_box_last_))
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

void Aplayer_controller::clients_notify_users_Implementation(
    Fuser_notification const & notification)
{
    if (auto * hud = ::hud(GetHUD())) {
        hud->notify_user(
            notification.title_, notification.message_, notification.button_);
    }
}

void Aplayer_controller::server_req_save_files_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;

    gm->publish_save_files();
}

bool Aplayer_controller::server_new_game_Validate(
    game_kind kind, FFilePath const & save)
{
    return hosting_or_sp();
}

void Aplayer_controller::server_new_game_Implementation(
    game_kind kind, FFilePath const & save)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    Ugame_instance::get()->game_kind(kind);
    Ugame_instance::get()->game_to_load(*save.FilePath);
    gm->ready_for_game();
}

void Aplayer_controller::client_galaxy_generation_update_Implementation(
    int percent_update)
{
    if (auto * hud = ::hud(GetHUD()))
        hud->generating_percent_update(percent_update);
}

bool Aplayer_controller::server_load_game_Validate(FString const & filename)
{
    return hosting_or_sp();
}

void Aplayer_controller::server_load_game_Implementation(
    FString const & filename)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;

    gm->load_and_start_game(filename);
}

bool Aplayer_controller::server_load_newest_game_Validate()
{
    return hosting_or_sp();
}

void Aplayer_controller::server_load_newest_game_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;

    gm->load_and_start_newest_game();
}

bool Aplayer_controller::server_quit_to_menu_Validate()
{
    return hosting_or_sp();
}

void Aplayer_controller::server_quit_to_menu_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->quit_to_menu();
}

bool Aplayer_controller::server_start_game_Validate(
    TArray<uint8> const & params)
{
    return hosting_or_sp();
}

void Aplayer_controller::server_start_game_Implementation(
    TArray<uint8> const & params)
{
    if (auto * hud = cast(GetHUD()))
        hud->remove_game_setup_widget();

    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->load_or_generate(params);
}

bool Aplayer_controller::server_save_game_Validate(FString const & filename)
{
    return hosting_or_sp();
}

void Aplayer_controller::server_save_game_Implementation(
    FString const & filename)
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->save_game(filename);
}

void Aplayer_controller::client_recv_initial_game_state_Implementation(
    int nation_id, TArray<uint8> const & state)
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Client: Received message from server (%d bytes)"),
        state.Num());

    nation_id_ = nation_id;

    client_gs_ = client_game_state(std::as_bytes(
        std::span(state.GetData(), state.GetData() + state.Num())));

    auto opt_nation = client_gs_.nation(nation_id_);
    check(opt_nation);
    auto opt_hex = home_hex(client_gs_, *opt_nation);
    check(opt_hex);
    auto const home_hc = opt_hex->coord;
    auto location = map_hex_position(home_hc, client_gs_.map_height());
    location.Z = 250.0f - map_actors_vertical_offset;

    auto * pawn = Cast<Acontroller_pawn>(GetPawn());
    check(pawn);
    pawn->start_game_at(location);

    server_confirm_received_initial_game_state();
}

bool Aplayer_controller::
    server_confirm_received_initial_game_state_Validate()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return false;
    return gm->waiting_for_client_to_receive_initial_game_state(nation_id_);
}

void Aplayer_controller::
    server_confirm_received_initial_game_state_Implementation()
{
    auto * gm = GetWorld()->GetAuthGameMode<Agame_mode>();
    if (!gm)
        return;
    gm->client_received_initial_game_state(nation_id_);
}

void Aplayer_controller::client_recv_day_updates_Implementation(
    TArray<uint8> const & state)
{
    // TODO
}

void Aplayer_controller::client_recv_month_updates_Implementation(
    TArray<uint8> const & state)
{
    // TODO
}

void Aplayer_controller::client_recv_year_updates_Implementation(
    TArray<uint8> const & state)
{
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

UInputMappingContext const & Aplayer_controller::input_mapping_context() const
{
    UInputMappingContext * imc = input_mapping_ctx_.Get();
    check(imc);
    return *imc;
}

TArray<FEnhancedActionKeyMapping>
Aplayer_controller::player_mappable_action_key_mappings() const
{
    ULocalPlayer * local_player = GetLocalPlayer();
    check(local_player);
    auto * input_sys =
        local_player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(input_sys);
    return input_sys->GetAllPlayerMappableActionKeyMappings();
}

TMap<FKey, FKey> Aplayer_controller::current_to_default_keys() const
{
    TMap<FKey, FKey> retval;

    if (ULocalPlayer * local_player = GetLocalPlayer()) {
        if (auto * input_sys =
                local_player
                    ->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
            if (UEnhancedInputUserSettings * user_settings =
                    input_sys->GetUserSettings()) {
                if (auto * profile = user_settings->GetActiveKeyProfile()) {
                    auto const & rows = profile->GetPlayerMappingRows();
                    for (auto && [_, row] : rows) {
                        for (auto && mapping : row.Mappings) {
                            retval.Add(
                                mapping.GetCurrentKey(),
                                mapping.GetDefaultKey());
                        }
                    }
                }
            }
        }
    }

    return retval;
}

Uui_defaults_t const & Aplayer_controller::ui_defaults()
{
    if (!ui_defaults_)
        ui_defaults_ = NewObject<Uui_defaults_t>(this, ui_defaults_class_);
    return *ui_defaults_;
}

Umaterials_t const & Aplayer_controller::materials()
{
    if (!materials_)
        materials_ = NewObject<Umaterials_t>(this, materials_class_);
    return *materials_;
}

Utextures_t const & Aplayer_controller::textures()
{
    if (!textures_)
        textures_ = NewObject<Utextures_t>(this, textures_class_);
    return *textures_;
}

Uaudio_assets_t const & Aplayer_controller::audio_assets()
{
    if (!audio_assets_)
        audio_assets_ = NewObject<Uaudio_assets_t>(this, audio_assets_class_);
    return *audio_assets_;
}

void Aplayer_controller::remap_key(FName name, FKey key)
{
    if (auto * local_player = GetLocalPlayer()) {
        if (auto * input_sys =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                    local_player)) {
            if (UEnhancedInputUserSettings * user_settings =
                    input_sys->GetUserSettings()) {
                FMapPlayerKeyArgs args = {};
                args.MappingName = name;
                args.Slot = EPlayerMappableKeySlot::First;
                args.NewKey = key;
                FGameplayTagContainer failure;
                user_settings->MapPlayerKey(args, failure);
                check(failure.IsEmpty());
            }
        }
    }
}

void Aplayer_controller::save_user_input_mappings()
{
    if (auto * local_player = GetLocalPlayer()) {
        if (auto * input_sys =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                    local_player)) {
            if (UEnhancedInputUserSettings * user_settings =
                    input_sys->GetUserSettings()) {
                user_settings->SaveSettings();
            }
        }
    }
}

void Aplayer_controller::showing_main_menu(bool b) { showing_main_menu_ = b; }

bool Aplayer_controller::showing_main_menu() const
{
    return showing_main_menu_;
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
        if (dragging(selection_box_first_, selection_box_last_)) {
            FVector2D xy;
            ProjectWorldLocationToScreen(p->GetActorLocation(), xy);
            if (!box.IsInsideOrOn(xy))
                continue;
        }
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

void Aplayer_controller::double_select(Amap_pawn_base * pawn)
{
    if (auto * map_system = Cast<Amap_system>(pawn)) {
        auto * camera_pawn = Cast<Acontroller_pawn>(GetPawn());
        check(camera_pawn);
        auto system = ::system(client_gs_, map_system->id());
        check(system);
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Moving into system view for system at %s"),
            *(FVector(system->world_pos_x, system->world_pos_y, 0) *
              ui_defaults().map_scale_)
                 .ToString());

        if (system_star_)
            system_star_->Destroy();
        for (auto p : system_planets_) {
            if (p)
                p->Destroy();
        }

        double const system_map_kms_per_world_unit = 500; // TODO -> constants

        // the sphere static mesh the star+planet BPs use is 200x200x200
        double const sphere_mesh_radius = 100;
        double const star_scale = system->star.solar_radii * sun_radius_km /
                                  system_map_kms_per_world_unit /
                                  sphere_mesh_radius;
        // TODO: Have a setting for nonproportional scaling of star+planets?

        auto const star_location =
            FVector(system->world_pos_x, system->world_pos_y, 0) *
            ui_defaults().map_scale_;
        system_star_ = GetWorld()->SpawnActor<AActor>(
            system_star_class_,
            star_location,
            FRotator(0, random_double(0, 360), 0),
            FActorSpawnParameters());
        configure_system_star(system_star_, system->star);
        system_star_->SetActorScale3D(FVector(star_scale));

        FAttachmentTransformRules const planet_attachment_rules(
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            false);

        system_planets_.SetNum(0);
        int missing = 0;
        for (int i = system->first_planet, last = system->last_planet;
             i != last;
             ++i) {
            if (auto planet = ::planet(client_gs_, i)) {
                FVector planet_location{};
                FMath::PolarToCartesian(
                    planet->orbit_au * km_per_au /
                        system_map_kms_per_world_unit,
                    1.0 * planet->orbital_pos_r,
                    planet_location.X,
                    planet_location.Y);
                planet_location /= 100.0; // TODO

                AActor * planet_actor = nullptr;
                if (planet->planet_type == planet_type_t::rocky) {
                    planet_actor = GetWorld()->SpawnActor<AActor>(
                        rocky_planet_class_,
                        star_location + planet_location,
                        FRotator(0, planet->axial_tilt_d, 0),
                        FActorSpawnParameters());

                    switch (planet->atmosphere_type) {
                    case atmosphere_type_t::reduced_type_a:
                    case atmosphere_type_t::carbon_rich_type_c:
                        configure_rocky_reduced_or_carbon_rich_planet(
                            planet_actor, *planet);
                        break;
                    case atmosphere_type_t::oxidized_type_b:
                        configure_rocky_oxidized_planet(
                            planet_actor,
                            *planet,
                            i,
                            population_of_planet_known_to_nation(
                                client_gs_, i, nation_id_),
                            mean_infrastructure_of_planet_known_to_nation(
                                client_gs_, i, nation_id_));
                        break;
                    case atmosphere_type_t::high_temperature:
                        configure_high_temperature_planet(
                            planet_actor, *planet);
                        break;
                    default:
                        UE_LOG(
                            LogTemp,
                            Error,
                            TEXT("Cannot render rocky planet with unexpected "
                                 "atmosphere type '%s'"),
                            *FString(std::format("{}", planet->atmosphere_type)
                                         .c_str()));
                    }
                } else {
                    planet_actor = GetWorld()->SpawnActor<AActor>(
                        gas_ice_giant_class_,
                        star_location + planet_location,
                        FRotator(0, planet->axial_tilt_d, 0),
                        FActorSpawnParameters());
                    if (planet->planet_type == planet_type_t::gas_giant)
                        configure_gas_giant_planet(planet_actor, *planet);
                    else
                        configure_ice_giant_planet(planet_actor, *planet);
                }

                double const scale = 20 * /*TODO*/ planet->radius_km /
                                     system_map_kms_per_world_unit /
                                     sphere_mesh_radius;
                planet_actor->SetActorScale3D(FVector(scale));
                planet_actor->AttachToActor(
                    system_star_, planet_attachment_rules);
                system_planets_.Add(planet_actor);
            } else {
                ++missing;
            }
        }
        check(missing == 0 || missing == (int)system_planets_.Num());

        check(map_transition_);
        map_transition_->to_system_map(
            star_location,
            camera_pawn->camera_location(),
            system_star_,
            system_planets_,
            system_fleets_);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Double click!")); // TODO
    }
}

bool Aplayer_controller::hosting_or_sp() const
{
    return cast(PlayerState)->player_id() == 0;
}
