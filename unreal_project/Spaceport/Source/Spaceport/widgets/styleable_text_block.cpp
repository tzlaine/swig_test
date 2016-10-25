// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_text_block.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_text_block)
    set_style(defaults.UTextBlock_style_path_);
    set_font(defaults.font_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_text_block)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_text_block)

void Ustyleable_text_block::set_font (FString const & font_path)
{
    ConstructorHelpers::FObjectFinder<UFont> font(*font_path);
    SetFont(font.Object->GetLegacySlateFontInfo());
}

void Ustyleable_text_block::set_wrap_width (float w)
{ WrapTextAt = w; }

void Ustyleable_text_block::apply_style ()
{
    STextBlock::FArguments args;
    args.TextStyle(style_asset_);
    MyTextBlock->SetTextStyle(args._TextStyle);
    Font.Size = args._TextStyle->Font.Size; // Sigh.
}
