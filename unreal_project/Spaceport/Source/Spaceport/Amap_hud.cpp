#include "Amap_hud.h"

#include <Widgets/SViewport.h>


Amap_hud::Amap_hud(FObjectInitializer const & init) : AHUD(init) {}

void Amap_hud::BeginPlay()
{
    Super::BeginPlay();

    // widget_ = SNew(TODO);
    // 
    // UGameViewportClient * viewport_client = GetWorld()->GetGameViewport();
    // viewport_client->AddViewportWidgetContent(widget_.ToSharedRef());
}
