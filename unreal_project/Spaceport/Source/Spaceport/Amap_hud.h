#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Amap_hud.generated.h"


UCLASS()
class Amap_hud : public AHUD
{
    GENERATED_BODY()

public:
    Amap_hud(FObjectInitializer const & init);

    // virtual void ShowHUD() override;
    // virtual void DrawHUD() override;

protected:
    virtual void BeginPlay() override;

    // TODO TSharedPtr<class TODO> widget_;
};
