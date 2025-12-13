#include "Aplaying_hud.h"
#include "Amap_pawn_base.h"
#include "Agame_state.h"
#include "game_instance.h"
#include "ui_defaults.h"
#include "utility.hpp"
#include "huds/Sgame_setup.h"
#include "huds/Sgenerating_galaxy.h"
#include "huds/Smain_menu.h"

#include <Widgets/SViewport.h>


Aplaying_hud::Aplaying_hud(FObjectInitializer const & init) : Ahud_t(init)
{
    in_game(true);
}

void Aplaying_hud::saves_list(TArray<FString> const & saves)
{
    Ahud_t::saves_list(saves);
    if (main_menu_)
        main_menu_->have_saves(!saves.IsEmpty());
}

void Aplaying_hud::saves_changed(TArray<Ffile_change> const & changes)
{
    Ahud_t::saves_changed(changes);
}

void Aplaying_hud::show_game_setup()
{
    allocate_widgets();
    push_modal(game_setup_);
}
void Aplaying_hud::remove_game_setup_widget()
{
    check(game_setup_);
    remove_widget(*game_setup_);
}

void Aplaying_hud::show_generating_galaxy()
{
    allocate_widgets();
    push_modal(generating_galaxy_);
    generating_progress_ = 0;
}
void Aplaying_hud::generating_percent_update(int u)
{
    if (!generating_galaxy_)
        return;
    generating_progress_ += u;
    generating_galaxy_->percent_complete(generating_progress_);
}
void Aplaying_hud::remove_generating_widget()
{
    check(generating_galaxy_);
    remove_widget(*generating_galaxy_);
}

void Aplaying_hud::set_selection_box_first(FVector2D first)
{
    selection_box_first_ = first;
    UE_LOG(
        LogTemp, Log, TEXT("selection box starts at %f,%f"), first.X, first.Y);
}

void Aplaying_hud::set_selection_box_last(FVector2D last)
{
    selection_box_last_ = last;
    UE_LOG(LogTemp, Log, TEXT("selection box stops at %f,%f"), last.X, last.Y);
}

TArray<Amap_pawn_base *> & Aplaying_hud::selected_in_box()
{
    return selected_pawns_;
}

void Aplaying_hud::BeginPlay()
{
    // Intentially NOT calling Super::BeginPlay().
    AHUD::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplaying_hud::BeginPlay()"));
    allocate_widgets();
    show_deferred_notifications(level::playing);
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplaying_hud::BeginPlay()"));
}

void Aplaying_hud::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplaying_hud::EndPlay()"));
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplaying_hud::EndPlay()"));
}

void Aplaying_hud::DrawHUD()
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

void Aplaying_hud::allocate_widgets()
{
    if (game_setup_)
        return;
    game_setup_ = SNew(Sgame_setup);
    generating_galaxy_ = SNew(Sgenerating_galaxy);
}
