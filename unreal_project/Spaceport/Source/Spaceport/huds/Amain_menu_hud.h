#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Amain_menu_hud.generated.h"


class Smain_menu;

UCLASS()
class Amain_menu_hud : public AHUD
{
    GENERATED_BODY()

public:
    Amain_menu_hud(FObjectInitializer const & init);

    void saves_available(bool b);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type reason) override;

    bool have_saves_ = false;
    TSharedPtr<Smain_menu> widget_;
};
