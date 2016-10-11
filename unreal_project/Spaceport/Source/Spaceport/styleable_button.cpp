// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_button.h"
#include "ui_defaults.h"


Ustyleable_button::Ustyleable_button () :
    style_asset_ (nullptr),
    built_ (false)
{
    auto const & defaults = ui_defaults();
    set_style(defaults.UButton_style_path_);
}

TSharedRef<SWidget> Ustyleable_button::RebuildWidget ()
{
    auto retval = Super::RebuildWidget();
    apply_style();
    built_ = true;
    return retval;
}

void Ustyleable_button::set_style (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_button::apply_style ()
{
    SButton::FArguments args;
    args.ButtonStyle(style_asset_);
    WidgetStyle = *args._ButtonStyle;
}
