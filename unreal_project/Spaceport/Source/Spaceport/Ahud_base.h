#pragma once

#include "huds/Sconfirm_dlg.h"

#include <functional>
#include <vector>

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Ahud_base.generated.h"


struct Ffile_change;
class Smain_menu;

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

    void escape_pressed();

    void do_after_confirming(std::function<void()> action,
                             FString title = TEXT("confirm_leave_game"),
                             FString message = TEXT("unsaved_progress_lost"),
                             FString yes_button = TEXT("leave_game"),
                             FString no_button = TEXT("cancel"));

protected:
    Smain_menu * main_menu() const;
    void show_modal(Shud_widget_base * widget);
    void hide_modal(Shud_widget_base * widget);

private:
    void allocate_widgets();

    TSharedPtr<Smain_menu> main_menu_;

    TSharedPtr<Sconfirm_dlg> confirm_dlg_;
    std::function<void()> action_after_confirmation_;
    Sconfirm_dlg::result confirm_dlg_result_ =
        Sconfirm_dlg::result::waiting_for_user;

    std::vector<Shud_widget_base *> modal_stack_;

    bool in_game_ = false;
};
