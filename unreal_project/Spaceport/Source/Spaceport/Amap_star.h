#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "Amap_star.generated.h"


UCLASS()
class Amap_star : public AActor
{
    GENERATED_BODY()

public:
    Amap_star();

    void BeginPlay() override;
    void Tick(float delta) override;
};
