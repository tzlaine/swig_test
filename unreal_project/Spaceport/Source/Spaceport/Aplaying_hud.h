#pragma once

#include "Ahud_base.h"
#include "huds/Sconfirm_dlg.h"

#include <functional>
#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Aplaying_hud.generated.h"


class Sgame_setup;
class Sgenerating_galaxy;
class Smain_menu;
class Shud_widget_base;

UCLASS()
class Aplaying_hud : public Ahud_base
{
    GENERATED_BODY()

public:
    Aplaying_hud(FObjectInitializer const & init);

    void Tick(float DeltaTime) override;

    void saves_list(TArray<FString> const & saves) override;
    void saves_changed(TArray<Ffile_change> const & changes) override;
    void escape_pressed() override;

    void do_after_confirming(std::function<void()> action,
                             FString title = TEXT("confirm_leave_game"),
                             FString message = TEXT("unsaved_progress_lost"),
                             FString yes_button = TEXT("leave_game"),
                             FString no_button = TEXT("cancel"));

    void show_main_menu();
    void hide_main_menu();

    void show_game_setup();
    void hide_game_setup();

    void show_generating_galaxy();
    void hide_generating_galaxy();
    void generating_percent_update(int p);

protected:
    void BeginPlay() override;
    void EndPlay(EEndPlayReason::Type reason) override;

    void allocate_widgets();
    void show_modal(Shud_widget_base * widget);
    void hide_modal(Shud_widget_base * widget);

    TSharedPtr<Sconfirm_dlg> confirm_dlg_;
    std::function<void()> action_after_confirmation_;
    Sconfirm_dlg::result confirm_dlg_result_ =
        Sconfirm_dlg::result::waiting_for_user;

    TSharedPtr<Smain_menu> main_menu_;
    TSharedPtr<Sgame_setup> game_setup_;
    TSharedPtr<Sgenerating_galaxy> generating_galaxy_;
    int generating_progress_ = 0;

    std::vector<Shud_widget_base *> modal_stack_;
};
