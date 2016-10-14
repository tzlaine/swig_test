// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "styleable_combo_box.h"
#include "ui_defaults.h"


STYLEABLE_WIDGET_IMPL_1(Ustyleable_combo_box)
    set_style(defaults.UComboBox_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyleable_combo_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyleable_combo_box)

void Ustyleable_combo_box::apply_style ()
{
    SComboBox<UObject*>::FArguments args;
    args.ComboBoxStyle(style_asset_);
    MyComboBox->Construct(args);
}
