#include "Amain_menu_hud.h"
#include "Agame_state.h"
#include "Smain_menu.h"
#include "utility.hpp"

#include <Kismet/GameplayStatics.h>
#include <Widgets/SViewport.h>


Amain_menu_hud::Amain_menu_hud(FObjectInitializer const & init) :
    Ahud_base(init)
{
    in_game(false);
}

void Amain_menu_hud::saves_list(TArray<FString> const & saves)
{
    Ahud_base::saves_list(saves);
    have_saves(!saves.IsEmpty());
}

void Amain_menu_hud::saves_changed(TArray<Ffile_change> const & changes)
{
    Ahud_base::saves_changed(changes);
}

void Amain_menu_hud::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_hud::BeginPlay()"));
    show_main_menu(false);
    show_deferred_notifications(level::start);
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_hud::BeginPlay()"));
}

void Amain_menu_hud::have_saves(bool b)
{
    if (main_menu_)
        main_menu_->have_saves(b);
}
