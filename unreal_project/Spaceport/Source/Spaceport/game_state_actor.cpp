// Fill out your copyright notice in the Description page of Project Settings.


#include "game_state_actor.h"

// Sets default values
Agame_state_actor::Agame_state_actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void Agame_state_actor::new_game()
{
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("Coming soon: galaxy generation."));
}