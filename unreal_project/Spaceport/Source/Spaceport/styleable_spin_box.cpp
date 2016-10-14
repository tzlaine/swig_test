// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_spin_box.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_spin_box)
    set_style(defaults.USpinBox_style_path_);
    set_font(defaults.font_path_);
    ForegroundColor = defaults.UComboBoxString_USpinBox_foreground_color_; // TODO: -> mutator
STYLEABLE_WIDGET_IMPL_2(Ustyleable_spin_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_spin_box)

void Ustyleable_spin_box::set_font (FString const & font_path)
{
    detail::FObjectFinder<UFont> font(*font_path);
    Font = font.Object->GetLegacySlateFontInfo();
}

void Ustyleable_spin_box::apply_style ()
{
    SSpinBox<float>::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
