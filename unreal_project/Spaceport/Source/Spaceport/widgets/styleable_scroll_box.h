// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/ScrollBox.h"

#include "styleable_scroll_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_scroll_box : public UScrollBox
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_scroll_box ();
    void set_scroll_style (FString const & style_path);

    STYLEABLE_WIDGET_PRIVATE();
    USlateWidgetStyleAsset* scroll_style_asset_;
};
