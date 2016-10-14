// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/TextBlock.h"

#include "styleable_text_block.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_text_block : public UTextBlock
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_text_block ();
    void set_font (FString const & font_path);

    STYLEABLE_WIDGET_PRIVATE();
};
