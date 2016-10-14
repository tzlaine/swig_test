// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_progress_bar.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_progress_bar)
    set_style(defaults.UProgressBar_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_progress_bar)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_progress_bar)

void Ustyleable_progress_bar::apply_style ()
{
    SProgressBar::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
