// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/ComboBox.h"

#include "styleable_combo_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_combo_box : public UComboBox
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_combo_box ();
    void set_font (FString const & font_path);

    STYLEABLE_WIDGET_PRIVATE();
};
