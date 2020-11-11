// Fill out your copyright notice in the Description page of Project Settings.


#include "styled_check_box.h"

STYLEABLE_WIDGET_IMPL_1(Ustyled_check_box)
set_style(defaults.UCheckBox_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyled_check_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyled_check_box)

void Ustyled_check_box::apply_style ()
{
    SCheckBox::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
