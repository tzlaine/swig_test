#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "Amap_hex.generated.h"


UCLASS()
class Amap_hex : public AActor
{
    GENERATED_BODY()

public:
    Amap_hex();

    void BeginPlay() override;
    void Tick(float delta) override;
};
