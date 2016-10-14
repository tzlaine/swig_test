// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/SpinBox.h"
#include "styleable_spin_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_spin_box : public USpinBox
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_spin_box ();
    void set_font (FString const & font_path);

    STYLEABLE_WIDGET_PRIVATE();
};
