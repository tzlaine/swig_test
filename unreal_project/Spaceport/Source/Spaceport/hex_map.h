// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "hex_map.generated.h"

UCLASS()
class SPACEPORT_API Ahex_map : public AActor
{
    GENERATED_BODY()

public:
    Ahex_map ();

    virtual void BeginPlay () override;

    virtual void Tick (float DeltaSeconds) override;

protected:
    void SpawnHex (int x, int y);

    UPROPERTY(EditAnywhere, Category = "Hex")
    TSubclassOf<class Ahex> hex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    int width;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    int height;
};
