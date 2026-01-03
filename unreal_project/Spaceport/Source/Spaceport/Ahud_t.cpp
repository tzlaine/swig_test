#include "Ahud_t.h"
#include "Agame_state.h"
#include "Amap_pawn_base.h"
#include "animations.hpp"
#include "game_instance.h"
#include "ui_defaults.h"
#include "utility.hpp"
#include "huds/Sgame_setup.h"
#include "huds/Sgenerating_galaxy.h"
#include "huds/Smain_menu.h"
#include "huds/Soptions.h"
#include "huds/Ssave_load_dlg.h"
#include "huds/Ssystem_map_ui.h"
#include "huds/Uactivatable_widget.h"

#include <Engine/GameViewportClient.h>


namespace {
    bool in_game(UWorld * w)
    {
        if (auto * gs = Cast<Agame_state>(UGameplayStatics::GetGameState(w)))
            return gs->playing_or_paused();
        return false;
    }
}

Ahud_t::Ahud_t(FObjectInitializer const & init) : AHUD(init) {}

void Ahud_t::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Ahud_t::BeginPlay()"));
    show_main_menu(false);
    UE_LOG(LogTemp, Log, TEXT("EXIT Ahud_t::BeginPlay()"));
}

void Ahud_t::Tick(float dt)
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

    animations_.tick(dt);
}

void Ahud_t::DrawHUD()
{
    Super::DrawHUD();

    if (selection_box_first_ == selection_box_last_)
        return;

    FLinearColor const color = ui_defaults().drag_selection_box_color_;

    DrawLine(
        selection_box_first_.X,
        selection_box_first_.Y,
        selection_box_first_.X,
        selection_box_last_.Y,
        color);
    DrawLine(
        selection_box_last_.X,
        selection_box_first_.Y,
        selection_box_last_.X,
        selection_box_last_.Y,
        color);
    DrawLine(
        selection_box_first_.X,
        selection_box_first_.Y,
        selection_box_last_.X,
        selection_box_first_.Y,
        color);
    DrawLine(
        selection_box_first_.X,
        selection_box_last_.Y,
        selection_box_last_.X,
        selection_box_last_.Y,
        color);

    selected_pawns_.Empty();
    GetActorsInSelectionRectangle<Amap_pawn_base>(
        selection_box_first_, selection_box_last_, selected_pawns_);
}

void Ahud_t::saves_list(TArray<FString> const & saves)
{
    UE_LOG(
        LogTemp,
        Log,
        TEXT("Ahud_t::saves_list(): new files: %s"),
        *FString::Join(saves, TEXT(", ")));
    if (save_load_dlg_)
        save_load_dlg_->saves_changed(saves);
    if (main_menu_)
        main_menu_->have_saves(!saves.IsEmpty());
}

void Ahud_t::saves_changed(TArray<Ffile_change> const & changes) {}

void Ahud_t::show_main_menu(bool in_game)
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

void Ahud_t::show_save_load_dlg(bool saving)
{
    save_load_dlg_ =
        SNew(Ssave_load_dlg).in_game(in_game(GetWorld())).saving(saving);
    push_modal(save_load_dlg_);
    if (auto * pc = player_controller())
        pc->server_req_save_files();
    // TODO: Sign up for dir watching while the main menu is up (in_game() ==
    // true only); cancel it afterward.
}

void Ahud_t::show_options()
{
    options_ = SNew(Soptions);
    push_modal(options_);
}

void Ahud_t::escape_pressed()
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

    if (in_game(GetWorld()))
        show_main_menu(true);
}

void Ahud_t::show_game_setup()
{
    allocate_widgets();
    push_modal(game_setup_);
}
void Ahud_t::remove_game_setup_widget()
{
    check(game_setup_);
    remove_widget(*game_setup_);
}

void Ahud_t::show_generating_galaxy()
{
    allocate_widgets();
    push_modal(generating_galaxy_);
    generating_progress_ = 0;
}
void Ahud_t::generating_percent_update(int u)
{
    if (!generating_galaxy_)
        return;
    generating_progress_ += u;
    generating_galaxy_->percent_complete(generating_progress_);
}
void Ahud_t::remove_generating_widget()
{
    check(generating_galaxy_);
    remove_widget(*generating_galaxy_);
}

void Ahud_t::do_after_confirming(std::function<void()> action,
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

void Ahud_t::notify_user(FString title, FString message, FString button)
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

void Ahud_t::notify_user(FString title, FText message, FString button)
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

void Ahud_t::push_modal(TSharedPtr<Shud_widget_base> widget)
{
    check(widget);
    g_content_shared_ptr = widget;
    modal_stack()->AddWidget<Uactivatable_widget>(
        Uactivatable_widget::StaticClass(), [](Uactivatable_widget & w) {
            w.content([] {
                check(g_content_shared_ptr);
                return g_content_shared_ptr.ToSharedRef();
            });
        });
}

void Ahud_t::remove_widget(Shud_widget_base & hud_widget)
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

void Ahud_t::remove_all_widgets()
{
    auto const all_widgets = modal_stack()->GetWidgetList();
    for (auto * activatable : all_widgets) {
        modal_stack()->RemoveWidget(*activatable);
    }
    main_menu_.Reset();
    hide_map_ui();
    if (auto * pc = player_controller())
        pc->showing_main_menu(false);
}

void Ahud_t::set_selection_box_first(FVector2D first)
{
    selection_box_first_ = first;
}

void Ahud_t::set_selection_box_last(FVector2D last)
{
    selection_box_last_ = last;
}

TArray<Amap_pawn_base *> & Ahud_t::selected_in_box() { return selected_pawns_; }

void Ahud_t::show_system_map_ui(int system_id)
{
    allocate_widgets();
    system_map_ui_->rebuild(system_id);
    use_map_ui(system_map_ui_);
    using namespace adobe::literals;
    animations_.start("map_ui_fade_in"_name);
}

void Ahud_t::hide_map_ui()
{
    allocate_widgets();
    using namespace adobe::literals;
    animations_.start("map_ui_fade_out"_name);
}

void Ahud_t::allocate_widgets()
{
    if (options_)
        return;

    options_ = SNew(Soptions);
    game_setup_ = SNew(Sgame_setup);
    generating_galaxy_ = SNew(Sgenerating_galaxy);
    system_map_ui_ = SNew(Ssystem_map_ui);

    using namespace adobe::literals;
    animations_.insert(
        "map_ui_fade_in"_name,
        animation(
            animation_kind::linear, ui_fade_in_time_s, [this](float alpha) {
                system_map_ui_->SetColorAndOpacity(
                    FLinearColor(1.0f, 1.0f, 1.0f, alpha));
            }));
    animations_
        .insert(
            "map_ui_fade_out"_name,
            animation(
                animation_kind::linear,
                ui_fade_in_time_s,
                [this](float alpha) {
                    system_map_ui_->SetColorAndOpacity(
                        FLinearColor(1.0f, 1.0f, 1.0f, 1 - alpha));
                }))
        .then([this] {
            use_map_ui({});
            system_map_ui_->reset();
        });
    // TODO: Same for galaxy map UI
}

UCommonActivatableWidgetStack * Ahud_t::modal_stack()
{
    check(stack_wrapper_);
    if (!stack_wrapper_->IsInViewport())
        stack_wrapper_->AddToViewport();
    check(stack_wrapper_->stack_);
    return stack_wrapper_->stack_;
}

void Ahud_t::use_map_ui(TSharedPtr<Shud_widget_base> widget)
{
    UGameViewportClient * viewport = ::world()->GetGameViewport();
    check(viewport);
    viewport->RemoveViewportWidgetContent(system_map_ui_->AsShared());
    // TODO viewport->RemoveViewportWidgetContent(galaxy_map_ui_);

    if (!widget)
        return;

    viewport->AddViewportWidgetContent(widget->AsShared());
}
