// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/ScrollBar.h"

#include "styleable_scroll_bar.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_scroll_bar : public UScrollBar
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_scroll_bar ();

    STYLEABLE_WIDGET_PRIVATE();
};
