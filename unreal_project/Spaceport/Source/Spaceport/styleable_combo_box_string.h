// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/ComboBoxString.h"

#include "styleable_combo_box_string.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_combo_box_string : public UComboBoxString
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_combo_box_string ();
    void set_font (FString const & font_path);

    STYLEABLE_WIDGET_PRIVATE();
};
