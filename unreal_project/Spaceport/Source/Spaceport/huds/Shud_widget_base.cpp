#include "Shud_widget_base.h"
#include "game_instance.h"

#include <Engine/World.h>
#include <Engine/GameViewportClient.h>
#include <Widgets/SViewport.h>


void Shud_widget_base::Construct(FArguments const & args) {}

bool Shud_widget_base::cancelable() { return false; }

void Shud_widget_base::cancel(UWorld * w) {}

void Shud_widget_base::show(UWorld * w)
{
    if (auto * viewport = w->GetGameViewport())
        viewport->AddViewportWidgetContent(AsShared());
}

void Shud_widget_base::hide(UWorld * w)
{
    if (auto * viewport = w->GetGameViewport())
        viewport->RemoveViewportWidgetContent(AsShared());
}
