#pragma once

#include "Amap_pawn_base_fwd.hpp"
#include "constants.hpp"
#include "game_data.hpp"
#include "model.hpp"
#include "user_notification.h"

#include <adobe/name.hpp>

#include <chrono>
#include <functional>
#include <map>
#include <span>

#include <CoreMinimal.h>
#include <UserSettings/EnhancedInputUserSettings.h>
#include <GameFramework/PlayerController.h>
#include "Aplayer_controller.generated.h"


class UMaterialInterface;
class UInputMappingContext;
class UInputAction;
class Uaudio_assets_t;
class Umaterials_t;
class Utextures_t;
class Uui_defaults_t;

UCLASS()
class Aplayer_controller : public APlayerController
{
    GENERATED_BODY()

public:
    Aplayer_controller();

    void BeginPlay() override;
    void SetupInputComponent() override;
    void Tick(float delta) override;

    UFUNCTION(NetMulticast, Reliable)
    void clients_notify_users(Fuser_notification const & notification);
    void clients_notify_users_Implementation(
        Fuser_notification const & notification);

    UFUNCTION(Server, Reliable)
    void server_req_save_files();
    void server_req_save_files_Implementation();

    UFUNCTION(Server, Reliable, WithValidation)
    void server_new_game(game_kind kind, FFilePath const & save);
    bool server_new_game_Validate(game_kind kind, FFilePath const & save);
    void server_new_game_Implementation(game_kind kind, FFilePath const & save);

    UFUNCTION(NetMulticast, Unreliable)
    void client_galaxy_generation_update(int percent_update);
    void client_galaxy_generation_update_Implementation(int percent_update);

    UFUNCTION(Server, Reliable, WithValidation)
    void server_load_game(FString const & filename);
    bool server_load_game_Validate(FString const & filename);
    void server_load_game_Implementation(FString const & filename);

    UFUNCTION(Server, Reliable, WithValidation)
    void server_load_newest_game();
    bool server_load_newest_game_Validate();
    void server_load_newest_game_Implementation();

    UFUNCTION(Server, Reliable, WithValidation)
    void server_quit_to_menu();
    bool server_quit_to_menu_Validate();
    void server_quit_to_menu_Implementation();

    UFUNCTION(Server, Reliable, WithValidation)
    void server_start_game(TArray<uint8> const & params);
    bool server_start_game_Validate(TArray<uint8> const & params);
    void server_start_game_Implementation(TArray<uint8> const & params);

    UFUNCTION(Server, Reliable)
    void server_save_game(FString const & filename);
    bool server_save_game_Validate(FString const & filename);
    void server_save_game_Implementation(FString const & filename);

    UFUNCTION(Client, Reliable)
    void
    client_recv_initial_game_state(int nation_id, TArray<uint8> const & state);
    void client_recv_initial_game_state_Implementation(
        int nation_id, TArray<uint8> const & state);

    UFUNCTION(Client, Reliable)
    void client_recv_day_updates(TArray<uint8> const & state);
    void client_recv_day_updates_Implementation(TArray<uint8> const & state);

    UFUNCTION(Client, Reliable)
    void client_recv_month_updates(TArray<uint8> const & state);
    void client_recv_month_updates_Implementation(TArray<uint8> const & state);

    UFUNCTION(Client, Reliable)
    void client_recv_year_updates(TArray<uint8> const & state);
    void client_recv_year_updates_Implementation(TArray<uint8> const & state);

    UFUNCTION(Server, Reliable)
    void server_toggle_pause();
    void server_toggle_pause_Implementation();

    UFUNCTION(Server, Reliable)
    void server_change_play_speed(int speed);
    void server_change_play_speed_Implementation(int speed);

    UInputMappingContext const & input_mapping_context() const;
    TArray<FEnhancedActionKeyMapping>
    player_mappable_action_key_mappings() const;
    TMap<FKey, FKey> current_to_default_keys() const;
    int nation_id() const { return nation_id_; }

    Uui_defaults_t const & ui_defaults();
    Umaterials_t const & materials();
    Utextures_t const & textures();
    Uaudio_assets_t const & audio_assets();
    void remap_key(FName name, FKey key);
    void save_user_input_mappings();
    void showing_main_menu(bool b);
    void nation_id(int id) { nation_id_ = id; }

protected:
    bool showing_main_menu() const;

private:
    enum struct deselect { no, yes };

    void dehover_all();
    void deselect_all();
    void hover(Amap_pawn_base * pawn);
    void select(Amap_pawn_base * pawn, deselect deselect_curr);
    void select(
        std::span<Amap_pawn_base *> pawns,
        deselect deselect_curr,
        map_pawn_kind kind);
    void select_in_box(map_pawn_kind selecting, deselect deselect_curr);
    void double_select(Amap_pawn_base * pawn);
    bool hosting_or_sp() const;

    int nation_id_ = nation_none;
    std::vector<Amap_pawn_base *> curr_hovers_;
    std::vector<Amap_pawn_base *> curr_selections_;
    bool keep_selected_key_down_ = false;
    bool alternate_selection_key_down_ = false;
    FVector2D selection_box_first_;
    FVector2D selection_box_last_;

    client_game_state client_gs_;

    bool showing_main_menu_ = false;

    // double-click tracking
    Amap_pawn_base * prev_select_target_ = nullptr;
    std::chrono::time_point<std::chrono::system_clock> prev_select_time_;

    // system view transition
    bool in_transition_ = false;
    float transition_progress_ = 0.0f;
    FVector initial_system_star_location_{};
    FVector final_system_star_location_{};

    // system view
    UPROPERTY()
    AActor * system_star_ = nullptr;
    UPROPERTY()
    TArray<AActor *> system_planets_;
    UPROPERTY()
    TArray<AActor *> system_fleets_;

    UPROPERTY(
        EditAnywhere,
        Category = "Asset classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Uui_defaults_t> ui_defaults_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "Asset classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Umaterials_t> materials_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "Asset classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Utextures_t> textures_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "Asset classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<Uaudio_assets_t> audio_assets_class_;

    UPROPERTY(
        EditAnywhere,
        Category = "BP classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> system_star_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "BP classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> rocky_planet_class_;
    UPROPERTY(
        EditAnywhere,
        Category = "BP classes",
        meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> gas_ice_giant_class_;

    UPROPERTY()
    TObjectPtr<Uui_defaults_t> ui_defaults_;
    UPROPERTY()
    TObjectPtr<Umaterials_t> materials_;
    UPROPERTY()
    TObjectPtr<Utextures_t> textures_;
    UPROPERTY()
    TObjectPtr<Uaudio_assets_t> audio_assets_;

    UPROPERTY(
        EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> input_mapping_ctx_;
    UPROPERTY(
        EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> menu_toggle_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> select_object_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> order_selected_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> incr_play_speed_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> decr_play_speed_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> pause_toggle_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> keep_selected_action_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Input",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> alternate_selection_action_;
};
