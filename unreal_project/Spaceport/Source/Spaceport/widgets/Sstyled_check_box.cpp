#include "Sstyled_check_box.h"

#include <ui_defaults.h>


void Sstyled_check_box::Construct(FArguments const & args_)
{
    SCheckBox::FArguments args(args_);
    args.Style(ui_defaults().CheckBox_style_.Get());
    SCheckBox::Construct(args);
}
