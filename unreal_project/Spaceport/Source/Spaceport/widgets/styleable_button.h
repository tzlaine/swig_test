// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/Button.h"

#include "styleable_button.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_button : public UButton
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyleable_button ();

    using signal_forward_fn_t = boost::function<void ()>;

    void set_signal_forward_fn(signal_forward_fn_t fn)
    { signal_forward_fn_ = fn; }

    UFUNCTION()
    void forward_signal () { signal_forward_fn_(); }

    STYLEABLE_WIDGET_PRIVATE();

    signal_forward_fn_t signal_forward_fn_;
};
