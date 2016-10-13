// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_check_box.h"
#include "ui_defaults.h"


Ustyleable_check_box::Ustyleable_check_box () :
    style_asset_ (nullptr),
    built_ (false)
{
    auto const & defaults = ui_defaults();
    set_style(defaults.UCheckBox_style_path_);
}

TSharedRef<SWidget> Ustyleable_check_box::RebuildWidget ()
{
    auto retval = Super::RebuildWidget();
    apply_style();
    built_ = true;
    return retval;
}

void Ustyleable_check_box::set_style (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_check_box::apply_style ()
{
    SCheckBox::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
