// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/EditableTextBox.h"

#include "styleable_editable_text_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_editable_text_box : public UEditableTextBox
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_editable_text_box ();
    void set_font (FString const & font_path);

    STYLEABLE_WIDGET_PRIVATE();
};
