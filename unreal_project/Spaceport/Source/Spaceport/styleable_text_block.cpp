// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_text_block.h"
#include "ui_defaults.h"


Ustyleable_text_block::Ustyleable_text_block ()
{
    auto const & defaults = ui_defaults();
    init(defaults.font_path_, defaults.UTextBlock_style_path_);
}

Ustyleable_text_block::Ustyleable_text_block (FText const & text, FString const & font_path, FString const & style_path)
{
    init(font_path, style_path);
    SetText(text);
}

TSharedRef<SWidget> Ustyleable_text_block::RebuildWidget()
{
    auto retval = Super::RebuildWidget();

    STextBlock::FArguments args;
    args.TextStyle(style_asset_);
    MyTextBlock->SetTextStyle(args._TextStyle);

    return retval;
}

void Ustyleable_text_block::init (FString const & font_path, FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<UFont> font(*font_path);
    SetFont(font.Object->GetLegacySlateFontInfo());

    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;
}
