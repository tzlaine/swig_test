// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "game_state_actor.generated.h"

UCLASS()
class SPACEPORT_API Agame_state_actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Agame_state_actor();

	void new_game();
};
