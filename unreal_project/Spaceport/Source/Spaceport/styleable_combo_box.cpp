// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_combo_box.h"
#include "ui_defaults.h"


Ustyleable_combo_box::Ustyleable_combo_box () :
    style_asset_ (nullptr),
    built_ (false)
{
    auto const & defaults = ui_defaults();
    set_style(defaults.UComboBox_style_path_);
}

TSharedRef<SWidget> Ustyleable_combo_box::RebuildWidget ()
{
    auto retval = Super::RebuildWidget();
    apply_style();
    built_ = true;
    return retval;
}

void Ustyleable_combo_box::set_style (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_combo_box::apply_style ()
{
    SComboBox<UObject*>::FArguments args;
    args.ComboBoxStyle(style_asset_);
    MyComboBox->Construct(args);
}
