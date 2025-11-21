#include "Ahud_base.h"
#include "Agame_state_base.h"
#include "game_instance.h"
#include "utility.hpp"
#include "huds/Smain_menu.h"
#include "huds/Ssave_load_dlg.h"


Ahud_base::Ahud_base(FObjectInitializer const & init) : AHUD(init) {}

void Ahud_base::Tick(float dt)
{
    Super::Tick(dt);

    std::erase_if(confirm_dlg_infos_, [this](auto const & info) {
        if (info.result_ == Sconfirm_dlg::result::waiting_for_user)
            return false;
        if (info.result_ == Sconfirm_dlg::result::yes)
            info.action_();
        hide_modal(info.dlg_);
        return true;
    });
}

void Ahud_base::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);
    UE_LOG(LogTemp, Log, TEXT("ENTER Ahud_base::EndPlay()"));
    for (auto & widget : modal_stack_) {
        if (auto w = widget.Pin())
            w->hide();
    }
    modal_stack_.clear();
    UE_LOG(LogTemp, Log, TEXT("EXIT Ahud_base::EndPlay()"));
}

void Ahud_base::saves_list(TArray<FString> const & saves)
{
    UE_LOG(
        LogTemp,
        Log,
        TEXT("Ahud_base::saves_list(): new files: %s"),
        *FString::Join(saves, TEXT(", ")));
    if (!save_load_dlg_)
        return;
    save_load_dlg_->saves_changed(saves);
}

void Ahud_base::saves_changed(TArray<Ffile_change> const & changes) {}

void Ahud_base::in_game(bool b)
{
    in_game_ = b;
}

void Ahud_base::show_main_menu()
{
    allocate_widgets();
    show_modal(main_menu_);

    bool saves = false;
    if (auto * gs = Cast<Agame_state_base>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }
    main_menu_->have_saves(saves);
}
void Ahud_base::hide_main_menu()
{
    allocate_widgets();
    hide_modal(main_menu_);
}

void Ahud_base::show_save_load_dlg(bool saving)
{
    save_load_dlg_ = SNew(Ssave_load_dlg).in_game(in_game_).saving(saving);
    show_modal(save_load_dlg_);
    if (auto * pc = player_controller_base())
        pc->server_req_save_files();
    // TODO: Sign up for dir watching while the main menu is up (in_game_ ==
    // true only); cancel it afterward.
}
void Ahud_base::hide_save_load_dlg()
{
    allocate_widgets();
    hide_modal(save_load_dlg_);
}

void Ahud_base::escape_pressed()
{
    if (!modal_stack_.empty()) {
        if (auto back = modal_stack_.back().Pin(); back->cancelable()) {
            back->cancel();
            hide_modal(back);
        }
        return;
    }

#if 0 // TODO
    if (modeless_dialog_with_focus) {
        if (modeless_dialog_with_focus->cancelable()) {
            modeless_dialog_with_focus->cancel();
            modeless_dialog_with_focus->hide();
            modeless_dialog_with_focus = nullptr; // TODO: Move focus to the next one?
            return;
        }
    }
#endif

    if (in_game_)
        show_main_menu();
}

void Ahud_base::do_after_confirming(std::function<void()> action,
                                    FString title,
                                    FString message,
                                    FString yes_button,
                                    FString no_button)
{
    check(action);
    confirm_dlg_infos_.emplace_back(
        nullptr, std::move(action), Sconfirm_dlg::result::waiting_for_user);
    TSharedPtr<Sconfirm_dlg> confirm_dlg =
        SNew(Sconfirm_dlg)
            .title(std::move(title))
            .message(std::move(message))
            .message_as_text({})
            .yes_button(std::move(yes_button))
            .no_button(std::move(no_button))
            .result_ptr(&confirm_dlg_infos_.back().result_);
    confirm_dlg_infos_.back().dlg_ = confirm_dlg;
    show_modal(confirm_dlg);
}

void Ahud_base::notify_user(FString title, FString message, FString button)
{
    confirm_dlg_infos_.push_back({});
    TSharedPtr<Sconfirm_dlg> confirm_dlg =
        SNew(Sconfirm_dlg)
            .title(std::move(title))
            .message(std::move(message))
            .message_as_text({})
            .yes_button(button)
            .no_button(button)
            .result_ptr(&confirm_dlg_infos_.back().result_);
    confirm_dlg_infos_.back().dlg_ = confirm_dlg;
    show_modal(confirm_dlg);
}

void Ahud_base::notify_user(FString title, FText message, FString button)
{
    confirm_dlg_infos_.push_back({});
    TSharedPtr<Sconfirm_dlg> confirm_dlg =
        SNew(Sconfirm_dlg)
            .title(std::move(title))
            .message({})
            .message_as_text(std::move(message))
            .yes_button(button)
            .no_button(button)
            .result_ptr(&confirm_dlg_infos_.back().result_);
    confirm_dlg_infos_.back().dlg_ = confirm_dlg;
    show_modal(confirm_dlg);
}

Smain_menu * Ahud_base::main_menu() const
{
    if (main_menu_)
        return main_menu_.Get();
    return nullptr;
}

void Ahud_base::show_modal(TSharedPtr<Shud_widget_base> widget)
{
    widget->show();
    modal_stack_.push_back(widget);
}

void Ahud_base::hide_modal(TSharedPtr<Shud_widget_base> widget)
{
    widget->hide();
    std::erase(modal_stack_, widget);
}

void Ahud_base::show_deferred_notifications(level l)
{
    auto notifications = Ugame_instance::get()->deferred_notifications(l);
    for (auto & n : notifications) {
        hud_base()->notify_user(std::move(n.title_), std::move(n.msg_));
    }
}

void Ahud_base::allocate_widgets()
{
    if (main_menu_)
        return;
    main_menu_ = SNew(Smain_menu).in_game(in_game_);
}
