#include "Amain_menu_hud.h"
#include "Smain_menu.h"

#include <Widgets/SViewport.h>


Amain_menu_hud::Amain_menu_hud(FObjectInitializer const & init) : AHUD(init) {}

void Amain_menu_hud::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Amain_menu_hud::BeginPlay()"));
    Super::BeginPlay();

    widget_ = SNew(Smain_menu).in_game(false);
    widget_->saves_available(have_saves_);

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

void Amain_menu_hud::saves_available(bool b)
{
    have_saves_ = b;
    if (widget_)
        widget_->saves_available(b);
}
