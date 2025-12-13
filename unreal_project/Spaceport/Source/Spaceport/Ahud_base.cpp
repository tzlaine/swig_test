#include "Ahud_base.h"
#include "Agame_state.h"
#include "game_instance.h"
#include "utility.hpp"
#include "huds/Smain_menu.h"
#include "huds/Ssave_load_dlg.h"
#include "huds/Soptions.h"
#include "huds/Uactivatable_widget.h"


Ahud_base::Ahud_base(FObjectInitializer const & init) : AHUD(init)
{
    in_game(false);
}

void Ahud_base::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_hud::BeginPlay()"));
    show_main_menu(false);
    show_deferred_notifications(level::start);
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_hud::BeginPlay()"));
}

void Ahud_base::Tick(float dt)
{
    Super::Tick(dt);

    std::erase_if(confirm_dlg_infos_, [this](auto const & info) {
        if (info.result_ == Sconfirm_dlg::result::waiting_for_user)
            return false;
        if (info.result_ == Sconfirm_dlg::result::yes)
            info.action_();
        remove_widget(*info.dlg_);
        return true;
    });
}

void Ahud_base::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);
    UE_LOG(LogTemp, Log, TEXT("ENTER Ahud_base::EndPlay()"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Ahud_base::EndPlay()"));
}

void Ahud_base::saves_list(TArray<FString> const & saves)
{
    UE_LOG(
        LogTemp,
        Log,
        TEXT("Ahud_base::saves_list(): new files: %s"),
        *FString::Join(saves, TEXT(", ")));
    if (save_load_dlg_)
        save_load_dlg_->saves_changed(saves);
    if (main_menu_)
        main_menu_->have_saves(!saves.IsEmpty());
}

void Ahud_base::saves_changed(TArray<Ffile_change> const & changes) {}

void Ahud_base::in_game(bool b)
{
    in_game_ = b;
}

void Ahud_base::show_main_menu(bool in_game)
{
    main_menu_ = SNew(Smain_menu).in_game(in_game);
    push_modal(main_menu_);
    bool saves = false;
    if (auto * gs = Cast<Agame_state>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }
    main_menu_->have_saves(saves);
    if (auto * pc = player_controller()) {
        pc->showing_main_menu(true);
        UE_LOG(LogTemp, Log, TEXT("Showing main menu"))
    }
}

void Ahud_base::show_save_load_dlg(bool saving)
{
    save_load_dlg_ = SNew(Ssave_load_dlg).in_game(in_game_).saving(saving);
    push_modal(save_load_dlg_);
    if (auto * pc = player_controller())
        pc->server_req_save_files();
    // TODO: Sign up for dir watching while the main menu is up (in_game_ ==
    // true only); cancel it afterward.
}

void Ahud_base::show_options()
{
    options_ = SNew(Soptions);
    push_modal(options_);
}

void Ahud_base::escape_pressed()
{
    UE_LOG(LogTemp, Warning, TEXT("ESCAPE!"));

    if (auto activatable = modal_stack()->GetActiveWidget()) {
        check(Cast<Uactivatable_widget>(activatable));
        if (Uactivatable_widget * w = Cast<Uactivatable_widget>(activatable)) {
            if (w->cancelable()) {
                w->cancel();
                if (auto * pc = player_controller();
                    pc && main_menu_ && w->wraps(*main_menu_)) {
                    pc->showing_main_menu(false);
                    UE_LOG(LogTemp, Log, TEXT("No longer showing main menu"));
                }
                modal_stack()->RemoveWidget(*activatable);
            }
            return;
        }
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
        show_main_menu(true);
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
    push_modal(confirm_dlg);
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
    push_modal(confirm_dlg);
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
    push_modal(confirm_dlg);
}

namespace {
    // Required by the inconvenient AddWidget API below.
    TSharedPtr<Shud_widget_base> g_content_shared_ptr;
}

void Ahud_base::push_modal(TSharedPtr<Shud_widget_base> widget)
{
    g_content_shared_ptr = widget;
    modal_stack()->AddWidget<Uactivatable_widget>(
        Uactivatable_widget::StaticClass(), [](Uactivatable_widget & w) {
            w.content([] {
                check(g_content_shared_ptr);
                return g_content_shared_ptr.ToSharedRef();
            });
        });
}

void Ahud_base::remove_widget(Shud_widget_base & hud_widget)
{
    for (auto * activatable : modal_stack()->GetWidgetList()) {
        check(Cast<Uactivatable_widget>(activatable));
        if (Cast<Uactivatable_widget>(activatable)->wraps(hud_widget)) {
            if (auto * pc = player_controller();
                pc && main_menu_ && &hud_widget == main_menu_.Get()) {
                pc->showing_main_menu(false);
                UE_LOG(LogTemp, Log, TEXT("No longer showing main menu"));
            }
            modal_stack()->RemoveWidget(*activatable);
            return;
        }
    }
}

void Ahud_base::remove_all_widgets()
{
    auto const & all_widgets = modal_stack()->GetWidgetList();
    for (auto * activatable : all_widgets) {
        modal_stack()->RemoveWidget(*activatable);
    }
}

void Ahud_base::show_deferred_notifications(level l)
{
    auto notifications = Ugame_instance::get()->deferred_notifications(l);
    for (auto & n : notifications) {
        notify_user(std::move(n.title_), std::move(n.msg_));
    }
}

UCommonActivatableWidgetStack * Ahud_base::modal_stack()
{
    check(stack_wrapper_);
    if (!stack_wrapper_->IsInViewport())
        stack_wrapper_->AddToViewport();
    check(stack_wrapper_->stack_);
    return stack_wrapper_->stack_;
}
