// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/TextBlock.h"

#include "styleable_text_block.generated.h"

/**
 * 
 */
UCLASS()
class SPACEPORT_API Ustyleable_text_block : public UTextBlock
{
	GENERATED_BODY()

public:
    Ustyleable_text_block ();

    virtual TSharedRef<SWidget> RebuildWidget () override;

    void set_style (FString const & style_path);
    void set_font (FString const & font_path);

private:
    void apply_style ();

    USlateWidgetStyleAsset* style_asset_;
    bool built_;
};
