// Fill out your copyright notice in the Description page of Project Settings.


#include "styled_button.h"

STYLEABLE_WIDGET_IMPL_1(Ustyled_button)
set_style(defaults.UButton_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyled_button)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyled_button)

void Ustyled_button::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();

    OnClicked.AddUniqueDynamic(this, &Ustyled_button::trampoline);
}

void Ustyled_button::trampoline()
{
    if (fn_)
      fn_();
}

void Ustyled_button::apply_style ()
{
    SButton::FArguments args;
    args.ButtonStyle(style_asset_);
    WidgetStyle = *args._ButtonStyle;
}
