// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_spin_box.h"
#include "ui_defaults.h"


Ustyleable_spin_box::Ustyleable_spin_box () :
    style_asset_ (nullptr),
    built_ (false)
{
    auto const & defaults = ui_defaults();
    set_style(defaults.USpinBox_style_path_);
    set_font(defaults.font_path_);
}

TSharedRef<SWidget> Ustyleable_spin_box::RebuildWidget ()
{
    auto retval = Super::RebuildWidget();
    apply_style();
    built_ = true;
    return retval;
}

void Ustyleable_spin_box::set_style (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_spin_box::set_font (FString const & font_path)
{
    ConstructorHelpers::FObjectFinder<UFont> font(*font_path);
    Font = font.Object->GetLegacySlateFontInfo();
}

void Ustyleable_spin_box::apply_style ()
{
    SSpinBox<float>::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
