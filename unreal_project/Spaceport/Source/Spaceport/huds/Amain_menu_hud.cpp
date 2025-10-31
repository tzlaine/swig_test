#include "Amain_menu_hud.h"
#include "Smain_menu.h"

#include <Widgets/SViewport.h>


Amain_menu_hud::Amain_menu_hud(FObjectInitializer const & init) : AHUD(init) {}

void Amain_menu_hud::BeginPlay()
{
    Super::BeginPlay();

    widget_ = SNew(Smain_menu).in_game(false);
    widget_->saves_available(have_saves_);

    UGameViewportClient * viewport_client = GetWorld()->GetGameViewport();
    viewport_client->AddViewportWidgetContent(widget_.ToSharedRef());
}

void Amain_menu_hud::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);

    GetWorld()->GetGameViewport()->RemoveViewportWidgetContent(
        widget_.ToSharedRef());
}

void Amain_menu_hud::saves_available(bool b)
{
    have_saves_ = b;
    if (widget_)
        widget_->saves_available(b);
}
