#include "Aplaying_hud.h"
#include "Agame_state_base.h"
#include "game_instance.h"
#include "utility.hpp"
#include "huds/Shud_widget_base.h"
#include "huds/Sgame_setup.h"
#include "huds/Sgenerating_galaxy.h"
#include "huds/Smain_menu.h"

#include <Widgets/SViewport.h>


Aplaying_hud::Aplaying_hud(FObjectInitializer const & init) : Ahud_base(init) {}

void Aplaying_hud::saves_list(TArray<FString> const & saves)
{
    if (main_menu_)
        main_menu_->have_saves(!saves.IsEmpty());
    // TODO: Notify save/load ui
}

void Aplaying_hud::saves_changed(TArray<Ffile_change> const & changes)
{
    // TODO: Notify save/load ui
}

void Aplaying_hud::escape_pressed()
{
    if (!modal_stack_.empty()) {
        if (modal_stack_.back()->cancelable()) {
            modal_stack_.back()->cancel(GetWorld());
            hide_modal(modal_stack_.back());
        }
        return;
    }

#if 0 // TODO
    if (nonmodal_dialog_with_focus) {
        if (nonmodal_dialog_with_focus->cancelable()) {
            nonmodal_dialog_with_focus->cancel(GetWorld());
            nonmodal_dialog_with_focus->hide();
            nonmodal_dialog_with_focus = nullptr; // TODO: Move focus to the next one?
            return;
        }
    }
#endif

    show_main_menu();
}

void Aplaying_hud::show_main_menu()
{
    allocate_widgets();
    show_modal(main_menu_.Get());

    bool saves = false;
    if (auto * gs = Cast<Agame_state_base>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }
    main_menu_->have_saves(saves);

    // TODO: Sign up for dir watching while the main menu is up; cancel it
    // afterward.  The results should go to the save/load ui.
}
void Aplaying_hud::hide_main_menu()
{
    allocate_widgets();
    hide_modal(main_menu_.Get());
}

void Aplaying_hud::show_game_setup()
{
    allocate_widgets();
    show_modal(game_setup_.Get());
}
void Aplaying_hud::hide_game_setup()
{
    allocate_widgets();
    hide_modal(game_setup_.Get());
}

void Aplaying_hud::show_generating_galaxy()
{
    allocate_widgets();
    show_modal(generating_galaxy_.Get());
    generating_progress_ = 0;
}
void Aplaying_hud::hide_generating_galaxy()
{
    allocate_widgets();
    hide_modal(generating_galaxy_.Get());
}
void Aplaying_hud::generating_percent_update(int u)
{
    if (!generating_galaxy_)
        return;
    generating_progress_ += u;
    generating_galaxy_->percent_complete(generating_progress_);
}

void Aplaying_hud::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplaying_hud::BeginPlay()"));
    Super::BeginPlay();

    allocate_widgets();

    UE_LOG(LogTemp, Log, TEXT("EXIT Aplaying_hud::BeginPlay()"));
}

void Aplaying_hud::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplaying_hud::EndPlay()"));
    Super::EndPlay(reason);

    for (auto * widget : modal_stack_) {
        widget->hide(GetWorld());
    }
    modal_stack_.clear();

    UE_LOG(LogTemp, Log, TEXT("EXIT Aplaying_hud::EndPlay()"));
}

void Aplaying_hud::allocate_widgets()
{
    if (main_menu_)
        return;
    main_menu_ = SNew(Smain_menu).in_game(true);
    game_setup_ = SNew(Sgame_setup);
    generating_galaxy_ = SNew(Sgenerating_galaxy);
}

void Aplaying_hud::show_modal(Shud_widget_base * widget)
{
    widget->show(GetWorld());
    modal_stack_.push_back(widget);
}

void Aplaying_hud::hide_modal(Shud_widget_base * widget)
{
    widget->hide(GetWorld());
    std::erase(modal_stack_, widget);
}
