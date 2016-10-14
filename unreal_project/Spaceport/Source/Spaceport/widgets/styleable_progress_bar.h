// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/ProgressBar.h"

#include "styleable_progress_bar.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_progress_bar : public UProgressBar
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_progress_bar ();

    STYLEABLE_WIDGET_PRIVATE();
};
