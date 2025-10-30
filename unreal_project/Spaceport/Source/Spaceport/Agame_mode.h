#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode.generated.h"


UCLASS()
class Agame_mode : public AGameModeBase
{
    GENERATED_BODY()

public:
    Agame_mode(FObjectInitializer const & init);
	
protected:
    virtual void BeginPlay() override;
};
