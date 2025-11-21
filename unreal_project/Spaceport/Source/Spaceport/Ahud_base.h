#pragma once

#include "game_instance.h"
#include "huds/Sconfirm_dlg.h"

#include <functional>
#include <list>
#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Ahud_base.generated.h"


struct Ffile_change;
class Smain_menu;
class Ssave_load_dlg;

UCLASS()
class Ahud_base : public AHUD
{
    GENERATED_BODY()

public:
    Ahud_base(FObjectInitializer const & init);

    void Tick(float dt) override;
    void EndPlay(EEndPlayReason::Type reason) override;

    virtual void saves_list(TArray<FString> const & saves);
    virtual void saves_changed(TArray<Ffile_change> const & changes);

    void in_game(bool b);

    void show_main_menu();
    void hide_main_menu();

    void show_save_load_dlg(bool saving);
    void hide_save_load_dlg();

    void escape_pressed();

    void do_after_confirming(std::function<void()> action,
                             FString title = TEXT("confirm_leave_game"),
                             FString message = TEXT("unsaved_progress_lost"),
                             FString yes_button = TEXT("leave_game"),
                             FString no_button = TEXT("cancel"));
    void notify_user(
        FString title, FString message, FString button = TEXT("ok"));
    void notify_user(
        FString title, FText message, FString button = TEXT("ok"));

protected:
    Smain_menu * main_menu() const;
    void show_modal(TSharedPtr<Shud_widget_base> widget);
    void hide_modal(TSharedPtr<Shud_widget_base> widget);
    void show_deferred_notifications(level l);

private:
    void allocate_widgets();

    TSharedPtr<Smain_menu> main_menu_;
    TSharedPtr<Ssave_load_dlg> save_load_dlg_;

    struct confirm_dlg_info
    {
        TSharedPtr<Sconfirm_dlg> dlg_;
        std::function<void()> action_;
        Sconfirm_dlg::result result_ =
            Sconfirm_dlg::result::waiting_for_user;
    };
    std::list<confirm_dlg_info> confirm_dlg_infos_;

    std::vector<TWeakPtr<Shud_widget_base>> modal_stack_;

    bool in_game_ = false;
};
