#include "Amain_menu_hud.h"
#include "Amain_menu_game_state.h"
#include "Smain_menu.h"
#include "utility.hpp"

#include <Kismet/GameplayStatics.h>
#include <Widgets/SViewport.h>


Amain_menu_hud::Amain_menu_hud(FObjectInitializer const & init) : AHUD(init) {}

void Amain_menu_hud::saves_list(TArray<FString> const & saves)
{
    have_saves(!saves.IsEmpty());
    // TODO: Notify the save/load ui
}

void Amain_menu_hud::saves_changed(TArray<Ffile_change> const & changes)
{
    bool saves = false;
    if (auto * gs = Cast<Amain_menu_game_state>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }
    have_saves(saves);
    // TODO: Notify the save/load ui
}

void Amain_menu_hud::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_hud::BeginPlay()"));
    Super::BeginPlay();

    bool saves = false;
    if (auto * gs = Cast<Amain_menu_game_state>(
            UGameplayStatics::GetGameState(GetWorld()))) {
        saves = !gs->saves_.IsEmpty();
    }

    widget_ = SNew(Smain_menu).in_game(false);
    widget_->have_saves(saves);

    UGameViewportClient * viewport_client = GetWorld()->GetGameViewport();
    if (viewport_client)
        viewport_client->AddViewportWidgetContent(widget_.ToSharedRef());
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_hud::BeginPlay()"));
}

void Amain_menu_hud::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_hud::EndPlay()"));
    Super::EndPlay(reason);

    if (auto * viewport = GetWorld()->GetGameViewport())
        viewport->RemoveViewportWidgetContent(widget_.ToSharedRef());
    UE_LOG(LogTemp, Log, TEXT("EXIT Amain_menu_hud::EndPlay()"));
}

void Amain_menu_hud::have_saves(bool b)
{
    if (widget_)
        widget_->have_saves(b);
}
