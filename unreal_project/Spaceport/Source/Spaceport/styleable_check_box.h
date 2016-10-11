// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/CheckBox.h"

#include "styleable_check_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_check_box : public UCheckBox
{
	GENERATED_BODY()

public:
    Ustyleable_check_box ();

    virtual TSharedRef<SWidget> RebuildWidget () override;

    void set_style (FString const & style_path);

private:
    void apply_style ();

    USlateWidgetStyleAsset* style_asset_;
    bool built_;
};
