#include "Sstyled_progress_bar.h"
#include <ui_defaults.h>


void Sstyled_progress_bar::Construct(FArguments const & args_)
{
    SProgressBar::FArguments args(args_);
    args.Style(ui_defaults().ProgressBar_style_.Get());
    SProgressBar::Construct(args);
}
