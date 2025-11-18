#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Ahud_base.generated.h"


struct Ffile_change;

UCLASS()
class Ahud_base : public AHUD
{
    GENERATED_BODY()

public:
    Ahud_base(FObjectInitializer const & init);

    virtual void saves_list(TArray<FString> const & saves);
    virtual void saves_changed(TArray<Ffile_change> const & changes);
    virtual void escape_pressed();
};
