// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/ComboBox.h"

#include "styleable_combo_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_combo_box : public UComboBox
{
    GENERATED_BODY()

public:
    Ustyleable_combo_box ();

    virtual TSharedRef<SWidget> RebuildWidget () override;

    void set_style (FString const & style_path);

private:
    void apply_style ();

    USlateWidgetStyleAsset* style_asset_;
    bool built_;
};
