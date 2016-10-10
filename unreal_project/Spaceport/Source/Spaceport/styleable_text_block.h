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
    Ustyleable_text_block (FText const & text, FString const & font_path, FString const & style_path);

    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void init (FString const & font_path, FString const & style_path);

    USlateWidgetStyleAsset* style_asset_;
};
