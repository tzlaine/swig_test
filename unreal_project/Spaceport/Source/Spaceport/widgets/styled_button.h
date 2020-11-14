// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ui_defaults.h>
#include <ui_util.h>
#include "CoreMinimal.h"
#include "Components/Button.h"
#include "styled_button.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyled_button : public UButton
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyled_button ();

    virtual void OnWidgetRebuilt() override;

    template<typename Fn>
    void connect(Fn && fn)
    { fn_ = fn; }

protected:
    UFUNCTION()
    void trampoline();
    
    STYLEABLE_WIDGET_PRIVATE();

    std::function<void ()> fn_;
};
