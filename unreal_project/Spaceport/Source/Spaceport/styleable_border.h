// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ui_util.h"
#include "Components/Border.h"

#include "styleable_border.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_border : public UBorder
{
    GENERATED_BODY()

public:
    virtual TSharedRef<SWidget> RebuildWidget () override;

    Ustyleable_border ();

    STYLEABLE_WIDGET_PRIVATE();
};
