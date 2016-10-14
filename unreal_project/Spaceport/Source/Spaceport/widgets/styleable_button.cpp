// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_button.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_button)
    set_style(defaults.UButton_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_button)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_button)

void Ustyleable_button::apply_style ()
{
    SButton::FArguments args;
    args.ButtonStyle(style_asset_);
    WidgetStyle = *args._ButtonStyle;
}
