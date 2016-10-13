// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_combo_box_string.h"
#include "ui_defaults.h"


Ustyleable_combo_box_string::Ustyleable_combo_box_string () :
    style_asset_ (nullptr),
    built_ (false)
{
    auto const & defaults = ui_defaults();
    set_style(defaults.UComboBox_style_path_);
    set_font(defaults.font_path_);
}

TSharedRef<SWidget> Ustyleable_combo_box_string::RebuildWidget ()
{
    auto retval = Super::RebuildWidget();
    apply_style();
    built_ = true;
    return retval;
}

void Ustyleable_combo_box_string::set_style (FString const & style_path)
{
    ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_combo_box_string::set_font (FString const & font_path)
{
    ConstructorHelpers::FObjectFinder<UFont> font(*font_path);
    Font = font.Object->GetLegacySlateFontInfo();
}

void Ustyleable_combo_box_string::apply_style ()
{
    SComboBox<TSharedPtr<FString>>::FArguments args;
    args.ComboBoxStyle(style_asset_);
    MyComboBox->Construct(args);
}
