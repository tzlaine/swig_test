// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "hex_component.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEPORT_API Uhex_component : public UActorComponent
{
    GENERATED_BODY()

public:	
    // Sets default values for this component's properties
    Uhex_component();
};
