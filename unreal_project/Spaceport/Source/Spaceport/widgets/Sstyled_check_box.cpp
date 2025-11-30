#include "Sstyled_check_box.h"

#include <ui_defaults.h>
#include <ui_util.h>


void Sstyled_check_box::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UCheckBox_style_path_);
    SCheckBox::FArguments args(args_);
    args.Style(style.Object);
    SCheckBox::Construct(args);
}
