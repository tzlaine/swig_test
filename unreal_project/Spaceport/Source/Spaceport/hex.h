// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "hex.generated.h"

UCLASS()
class SPACEPORT_API Ahex : public AActor
{
    GENERATED_BODY()
public: 
    Ahex();

    inline class UStaticMeshComponent* mesh() const
    { return mesh_; }

    inline void set_position(int x, int y)
    {
        x_ = x;
        y_ = y;
    }

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex", meta = (AllowPrivateAccess = "true"))
    class UStaticMeshComponent* mesh_;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex", meta = (AllowPrivateAccess = "true"))
    int x_;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex", meta = (AllowPrivateAccess = "true"))
    int y_;
};
