#include "Sstyled_border.h"

#include <ui_defaults.h>


void Sstyled_border::Construct(FArguments const & args_)
{
    SBorder::FArguments args(args_);
    args.BorderImage(&ui_defaults().border_background_);
    SBorder::Construct(args);
}
