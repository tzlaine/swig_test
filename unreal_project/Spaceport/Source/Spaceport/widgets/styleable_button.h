// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/Button.h"

#include "styleable_button.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_button :
    public UButton,
    public signal_forwarder
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_button ();

    UFUNCTION()
    void forward_signal () { signal_forward_fn_(); }

    STYLEABLE_WIDGET_PRIVATE();
};
