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

    using signal_forward_fn_t = boost::function<void (bool)>;

    void set_signal_forward_fn(signal_forward_fn_t fn)
    { signal_forward_fn_ = fn; }

    UFUNCTION()
    void forward_signal (bool b) { signal_forward_fn_(b); }

    STYLEABLE_WIDGET_PRIVATE();

    signal_forward_fn_t signal_forward_fn_;
};
