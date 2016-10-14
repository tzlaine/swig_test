// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_editable_text.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_editable_text)
    set_style(defaults.UEditableText_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_editable_text)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_editable_text)

void Ustyleable_editable_text::apply_style ()
{
    SEditableText::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
