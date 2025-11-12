#include "Sstyled_progress_bar.h"


void Sstyled_progress_bar::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UProgressBar_style_path_);
    SProgressBar::FArguments args(args_);
    args.Style(style.Object);
    SProgressBar::Construct(args);
}
