// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_editable_text_box.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_editable_text_box)
    set_style(defaults.UEditableTextBox_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_editable_text_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_editable_text_box)

void Ustyleable_editable_text_box::apply_style ()
{
    SEditableTextBox::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
