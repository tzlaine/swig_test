#include "styled_spin_box.h"

STYLEABLE_WIDGET_IMPL_1(Ustyled_spin_box)
set_style(defaults.USpinBox_style_path_);
STYLEABLE_WIDGET_IMPL_2(Ustyled_spin_box)

STYLEABLE_WIDGET_IMPL_SET_STYLE(Ustyled_spin_box)

void Ustyled_spin_box::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();
}

void Ustyled_spin_box::apply_style ()
{
    SSpinBox<int>::FArguments args;
    args.Style(style_asset_);
    WidgetStyle = *args._Style;
}
