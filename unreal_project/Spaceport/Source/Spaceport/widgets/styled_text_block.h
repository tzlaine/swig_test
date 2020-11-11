// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ui_defaults.h>
#include <ui_util.h>
#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "styled_text_block.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyled_text_block : public UTextBlock
{
	GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyled_text_block();

    STYLEABLE_WIDGET_PRIVATE();
};
