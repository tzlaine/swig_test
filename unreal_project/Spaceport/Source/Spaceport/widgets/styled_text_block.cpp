// Fill out your copyright notice in the Description page of Project Settings.


#include "styled_text_block.h"

STYLEABLE_WIDGET_IMPL_1(Ustyled_text_block)
set_style(defaults.UCheckBox_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyled_text_block)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyled_text_block)

void Ustyled_text_block::apply_style ()
{
    auto const& defaults = ui_defaults();
    detail::FObjectFinder<UFont> ufont(*defaults.font_path_);
    Font = FSlateFontInfo(ufont.Object, defaults.font_size_);
}
