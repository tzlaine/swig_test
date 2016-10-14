// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/CheckBox.h"

#include "styleable_check_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_check_box : public UCheckBox
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_check_box ();

    STYLEABLE_WIDGET_PRIVATE();
};
