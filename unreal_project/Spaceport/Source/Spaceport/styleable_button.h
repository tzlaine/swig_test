// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/Button.h"

#include "styleable_button.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_button : public UButton
{
	GENERATED_BODY()

public:
    Ustyleable_button ();

    virtual TSharedRef<SWidget> RebuildWidget () override;

    void set_style (FString const & style_path);

private:
    void apply_style ();

    USlateWidgetStyleAsset* style_asset_;
    bool built_;
};
