// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/EditableText.h"

#include "styleable_editable_text.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_editable_text : public UEditableText
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_editable_text ();

    STYLEABLE_WIDGET_PRIVATE();
};
