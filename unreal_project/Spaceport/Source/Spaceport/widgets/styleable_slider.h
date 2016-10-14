// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/Slider.h"

#include "styleable_slider.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_slider : public USlider
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_slider ();

    STYLEABLE_WIDGET_PRIVATE();
};
