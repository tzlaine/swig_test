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
    Ustyleable_button (FString const & style_path);

    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void init (FString const & style_path);

    USlateWidgetStyleAsset* style_asset_;
};
