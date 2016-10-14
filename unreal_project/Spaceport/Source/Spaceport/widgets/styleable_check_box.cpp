// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_check_box.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_check_box)
    set_style(defaults.UCheckBox_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_check_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_check_box)

void Ustyleable_check_box::apply_style ()
{
    SCheckBox::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
