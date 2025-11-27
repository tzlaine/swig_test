#include "Shud_widget_base.h"
#include "game_instance.h"

#include <Engine/World.h>
#include <Engine/GameViewportClient.h>
#include <Widgets/SViewport.h>


void Shud_widget_base::Construct(FArguments const & args) {}

bool Shud_widget_base::cancelable() { return false; }

void Shud_widget_base::cancel() {}
