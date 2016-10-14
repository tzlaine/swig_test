// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/Button.h"

#include "styleable_button.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_button : public UButton
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_button ();

    STYLEABLE_WIDGET_PRIVATE();
};
