// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex.h"


// Sets default values
Ahex::Ahex()
{
    PrimaryActorTick.bCanEverTick = false;
    mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("hex_mesh"));
    RootComponent = mesh_;
}
