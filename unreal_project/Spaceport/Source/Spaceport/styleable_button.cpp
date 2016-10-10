// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_button.h"
#include "ui_defaults.h"


Ustyleable_button::Ustyleable_button ()
{
    auto const & defaults = ui_defaults();
    init(defaults.UButton_style_path_);
}

Ustyleable_button::Ustyleable_button (FString const & style_path)
{ init(style_path); }

TSharedRef<SWidget> Ustyleable_button::RebuildWidget()
{
    auto retval = Super::RebuildWidget();

    SButton::FArguments args;
    args.ButtonStyle(style_asset_);
    WidgetStyle = *args._ButtonStyle;

    return retval;
}

void Ustyleable_button::init (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;
}
