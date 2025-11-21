#include "Aplaying_hud.h"
#include "Agame_state_base.h"
#include "game_instance.h"
#include "utility.hpp"
#include "huds/Sgame_setup.h"
#include "huds/Sgenerating_galaxy.h"
#include "huds/Smain_menu.h"

#include <Widgets/SViewport.h>


Aplaying_hud::Aplaying_hud(FObjectInitializer const & init) : Ahud_base(init)
{
    in_game(true);
}

void Aplaying_hud::saves_list(TArray<FString> const & saves)
{
    Ahud_base::saves_list(saves);
    if (main_menu())
        main_menu()->have_saves(!saves.IsEmpty());
}

void Aplaying_hud::saves_changed(TArray<Ffile_change> const & changes)
{
    Ahud_base::saves_changed(changes);
}

void Aplaying_hud::show_game_setup()
{
    allocate_widgets();
    show_modal(game_setup_);
}
void Aplaying_hud::hide_game_setup()
{
    allocate_widgets();
    hide_modal(game_setup_);
}

void Aplaying_hud::show_generating_galaxy()
{
    allocate_widgets();
    show_modal(generating_galaxy_);
    generating_progress_ = 0;
}
void Aplaying_hud::hide_generating_galaxy()
{
    allocate_widgets();
    hide_modal(generating_galaxy_);
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
    Super::BeginPlay();
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

void Aplaying_hud::allocate_widgets()
{
    if (game_setup_)
        return;
    game_setup_ = SNew(Sgame_setup);
    generating_galaxy_ = SNew(Sgenerating_galaxy);
}
