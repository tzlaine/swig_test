#include "Amain_menu_hud.h"
#include "Agame_state_base.h"
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

    bool saves = false;
    if (auto * gs = Cast<Agame_state_base>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }

    widget_ = SNew(Smain_menu).in_game(false);
    widget_->have_saves(saves);
    show_modal(widget_);
    show_deferred_notifications(level::start);

    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_hud::BeginPlay()"));
}

void Amain_menu_hud::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_hud::EndPlay()"));
    hide_modal(widget_);
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_hud::EndPlay()"));
}

void Amain_menu_hud::have_saves(bool b)
{
    if (widget_)
        widget_->have_saves(b);
}
