#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Amain_menu_hud.generated.h"


UCLASS()
class Amain_menu_hud : public AHUD
{
    GENERATED_BODY()

public:
    Amain_menu_hud(FObjectInitializer const & init);

    // virtual void ShowHUD() override;
    // virtual void DrawHUD() override;

protected:
    virtual void BeginPlay() override;

    TSharedPtr<class Smain_menu> widget_;
};
