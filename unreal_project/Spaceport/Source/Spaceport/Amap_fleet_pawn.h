#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Pawn.h>
#include "Amap_fleet_pawn.generated.h"


UCLASS()
class Amap_fleet_pawn : public APawn
{
    GENERATED_BODY()

public:
    Amap_fleet_pawn();

    void BeginPlay() override;
    void Tick(float DeltaTime) override;
    void SetupPlayerInputComponent(
        class UInputComponent * PlayerInputComponent) override;
};
