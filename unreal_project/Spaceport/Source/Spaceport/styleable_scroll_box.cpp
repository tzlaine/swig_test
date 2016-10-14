// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_scroll_box.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_scroll_box)
    set_style(defaults.UScrollBox_style_path_);
    set_scroll_style(defaults.UScrollBar_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_scroll_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_scroll_box)

void Ustyleable_scroll_box::set_scroll_style (FString const & style_path)
{
    detail::FObjectFinder<USlateWidgetStyleAsset> style(*style_path);
    scroll_style_asset_ = style.Object;

    if (built_)
        apply_style();
}

void Ustyleable_scroll_box::apply_style ()
{
    SScrollBox::FArguments args;
    args.Style(style_asset_);
    args.ScrollBarStyle(scroll_style_asset_);
    WidgetStyle = *args._Style;
    WidgetBarStyle = *args._ScrollBarStyle;
}
