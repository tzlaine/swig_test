// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_combo_box_string.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_combo_box_string)
    set_style(defaults.UComboBox_style_path_);
    set_font(defaults.font_path_);
    ForegroundColor = defaults.UComboBoxString_USpinBox_foreground_color_; // TODO: -> mutator
STYLEABLE_WIDGET_IMPL_2(Ustyleable_combo_box_string)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_combo_box_string)

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
