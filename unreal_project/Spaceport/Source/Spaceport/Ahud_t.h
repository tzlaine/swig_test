#pragma once

#include "Ucommonui_stack_wrapper.h"
#include "Agame_mode_fwd.h"
#include "animations.hpp"
#include "game_instance.h"
#include "huds/Sconfirm_dlg.h"

#include <functional>
#include <list>
#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Ahud_t.generated.h"


class Amap_pawn_base;
struct Ffile_change;
class Sgame_setup;
class Sgenerating_galaxy;
class Smain_menu;
class Ssave_load_dlg;
class Soptions;
class Shud_widget_base;
class Shud_widget_base;
class Ssystem_map_ui;
struct client_game_state;
struct date_t;

UCLASS()
class Ahud_t : public AHUD
{
    GENERATED_BODY()

    struct confirm_dlg_info
    {
        void then(std::function<void()> action) { action_ = std::move(action); }

        TSharedPtr<Sconfirm_dlg> dlg_;
        std::function<void()> action_;
        Sconfirm_dlg::result result_ = Sconfirm_dlg::result::waiting_for_user;
    };

public:
    Ahud_t(FObjectInitializer const & init);

    void BeginPlay() override;
    void Tick(float dt) override;
    void DrawHUD() override;

    void saves_list(TArray<FString> const & saves);
    void saves_changed(TArray<Ffile_change> const & changes);

    void play_speed_changed(int play_speed);
    void play_state_changed(play_state ps);
    void date_changed(date_t const & date);

    void initial_game_state(client_game_state const & gs);

    void show_main_menu(bool in_game);
    void show_save_load_dlg(bool saving);
    void show_options();

    void escape_pressed();

    void show_game_setup();
    void remove_game_setup_widget();

    void show_generating_galaxy();
    void generating_percent_update(int p);
    void remove_generating_widget();

    void do_after_confirming(std::function<void()> action,
                             FString title = TEXT("confirm_leave_game"),
                             FString message = TEXT("unsaved_progress_lost"),
                             FString yes_button = TEXT("leave_game"),
                             FString no_button = TEXT("cancel"));

    void
    notify_user(FString title, FString message, FString button = TEXT("ok"));
    void notify_user(FString title, FText message, FString button = TEXT("ok"));

    void push_modal(TSharedPtr<Shud_widget_base> widget);
    void remove_widget(Shud_widget_base & w);
    // TODO: Need to handle the modeless case.
    void remove_all_widgets();

    void set_selection_box_first(FVector2D first);
    void set_selection_box_last(FVector2D last);
    TArray<Amap_pawn_base *> & selected_in_box();

    // Modeless UIs
    void show_system_map_ui(int system_id);
    // TODO void show_galaxy_map_ui();
    void hide_map_ui();

private:
    void allocate_widgets();
    UCommonActivatableWidgetStack * modal_stack();
    void use_map_ui(TSharedPtr<Shud_widget_base> widget);

    animations animations_;

    TSharedPtr<Smain_menu> main_menu_;
    TSharedPtr<Ssave_load_dlg> save_load_dlg_;
    TSharedPtr<Soptions> options_;
    TSharedPtr<Sgame_setup> game_setup_;
    TSharedPtr<Sgenerating_galaxy> generating_galaxy_;
    int generating_progress_ = 0;

    TSharedPtr<Ssystem_map_ui> system_map_ui_;

    std::list<confirm_dlg_info> confirm_dlg_infos_;

    FVector2D selection_box_first_;
    FVector2D selection_box_last_;
    TArray<Amap_pawn_base *> selected_pawns_;

    UPROPERTY(
        EditAnywhere, Category = "ui", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<Ucommonui_stack_wrapper> stack_wrapper_;
};
