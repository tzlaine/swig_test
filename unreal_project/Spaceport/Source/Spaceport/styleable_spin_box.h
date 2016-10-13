// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/SpinBox.h"
#include "styleable_spin_box.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_spin_box : public USpinBox
{
    GENERATED_BODY()

public:
    Ustyleable_spin_box ();

    virtual TSharedRef<SWidget> RebuildWidget () override;

    void set_style (FString const & style_path);
    void set_font (FString const & font_path);

private:
    void apply_style ();

    USlateWidgetStyleAsset* style_asset_;
    bool built_;
};
