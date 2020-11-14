// Fill out your copyright notice in the Description page of Project Settings.


#include "main_menu.h"

void Umain_menu::NativeConstruct()
{
    Super::NativeConstruct();
    if (continue_bn) {
        continue_bn->connect([]{
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, TEXT("CLICK!"));
            // TODO
        });
    }
}