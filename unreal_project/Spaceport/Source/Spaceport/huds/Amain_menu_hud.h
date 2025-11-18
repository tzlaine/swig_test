#pragma once

#include "Ahud_base.h"

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Amain_menu_hud.generated.h"


class Smain_menu;
struct Ffile_change;

UCLASS()
class Amain_menu_hud : public Ahud_base
{
    GENERATED_BODY()

public:
    Amain_menu_hud(FObjectInitializer const & init);

    void saves_list(TArray<FString> const & saves) override;
    void saves_changed(TArray<Ffile_change> const & changes) override;
    void escape_pressed() override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type reason) override;

    void have_saves(bool b);

    TSharedPtr<Smain_menu> widget_;
};
