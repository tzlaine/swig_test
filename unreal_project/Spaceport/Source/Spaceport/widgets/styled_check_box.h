// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ui_defaults.h>
#include <ui_util.h>
#include "CoreMinimal.h"
#include "Components/CheckBox.h"
#include "styled_check_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyled_check_box : public UCheckBox
{
	GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyled_check_box();

    STYLEABLE_WIDGET_PRIVATE();
};
