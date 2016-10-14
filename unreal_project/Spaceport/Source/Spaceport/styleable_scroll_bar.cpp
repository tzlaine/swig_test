// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_scroll_bar.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_scroll_bar)
    set_style(defaults.UScrollBar_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_scroll_bar)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_scroll_bar)

void Ustyleable_scroll_bar::apply_style ()
{
    SScrollBar::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
