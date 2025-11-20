#include "Ahud_base.h"
#include "Agame_state_base.h"
#include "game_instance.h"
#include "huds/Smain_menu.h"


Ahud_base::Ahud_base(FObjectInitializer const & init) : AHUD(init) {}

void Ahud_base::Tick(float dt)
{
    Super::Tick(dt);

    if (!confirm_dlg_)
        return;

    if (confirm_dlg_result_ != Sconfirm_dlg::result::waiting_for_user) {
        if (confirm_dlg_result_ == Sconfirm_dlg::result::yes)
            action_after_confirmation_();
        hide_modal(modal_stack_.back());
        confirm_dlg_.Reset();
    }
}

void Ahud_base::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Ahud_base::EndPlay()"));
    Super::EndPlay(reason);

    for (auto * widget : modal_stack_) {
        widget->hide(GetWorld());
    }
    modal_stack_.clear();

    UE_LOG(LogTemp, Log, TEXT("EXIT Ahud_base::EndPlay()"));
}

void Ahud_base::saves_list(TArray<FString> const & saves) {}

void Ahud_base::saves_changed(TArray<Ffile_change> const & changes) {}

void Ahud_base::in_game(bool b)
{
    in_game_ = b;
}

void Ahud_base::show_main_menu()
{
    allocate_widgets();
    show_modal(main_menu_.Get());

    bool saves = false;
    if (auto * gs = Cast<Agame_state_base>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }
    main_menu_->have_saves(saves);

    // TODO: Sign up for dir watching while the main menu is up (in_game_ ==
    // true only); cancel it afterward.  The results should go to the
    // save/load ui.
}
void Ahud_base::hide_main_menu()
{
    allocate_widgets();
    hide_modal(main_menu_.Get());
}

void Ahud_base::escape_pressed()
{
    if (!modal_stack_.empty()) {
        if (modal_stack_.back()->cancelable()) {
            modal_stack_.back()->cancel(GetWorld());
            hide_modal(modal_stack_.back());
        }
        return;
    }

#if 0 // TODO
    if (modeless_dialog_with_focus) {
        if (modeless_dialog_with_focus->cancelable()) {
            modeless_dialog_with_focus->cancel(GetWorld());
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
    check(!confirm_dlg_);
    action_after_confirmation_ = std::move(action);
    confirm_dlg_result_ = Sconfirm_dlg::result::waiting_for_user;
    confirm_dlg_ = SNew(Sconfirm_dlg)
                       .title(std::move(title))
                       .message(std::move(message))
                       .yes_button(std::move(yes_button))
                       .no_button(std::move(no_button))
                       .result_ptr(&confirm_dlg_result_);
    show_modal(confirm_dlg_.Get());
}

Smain_menu * Ahud_base::main_menu() const
{
    if (main_menu_)
        return main_menu_.Get();
    return nullptr;
}

void Ahud_base::show_modal(Shud_widget_base * widget)
{
    widget->show(GetWorld());
    modal_stack_.push_back(widget);
}

void Ahud_base::hide_modal(Shud_widget_base * widget)
{
    widget->hide(GetWorld());
    std::erase(modal_stack_, widget);
}

void Ahud_base::allocate_widgets()
{
    if (main_menu_)
        return;
    main_menu_ = SNew(Smain_menu).in_game(in_game_);
}
