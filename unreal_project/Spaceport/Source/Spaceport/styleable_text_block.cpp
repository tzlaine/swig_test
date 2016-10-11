// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_text_block.h"
#include "ui_defaults.h"


Ustyleable_text_block::Ustyleable_text_block () :
    style_asset_ (nullptr),
    built_ (false)
{
    auto const & defaults = ui_defaults();
    set_font(defaults.font_path_);
    set_style(defaults.UTextBlock_style_path_);
}

TSharedRef<SWidget> Ustyleable_text_block::RebuildWidget ()
{
    auto retval = Super::RebuildWidget();
    apply_style();
    built_ = true;
    return retval;
}

void Ustyleable_text_block::set_font (FString const & font_path)
{
    ConstructorHelpers::FObjectFinder<UFont> font(*font_path);
    SetFont(font.Object->GetLegacySlateFontInfo());
}

void Ustyleable_text_block::set_style (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_text_block::apply_style ()
{
    STextBlock::FArguments args;
    args.TextStyle(style_asset_);
    MyTextBlock->SetTextStyle(args._TextStyle);
}
