#include "Sstyled_text_block.h"

#include <ui_defaults.h>
#include <ui_util.h>


void Sstyled_text_block::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UTextBlock_style_path_);
    STextBlock::FArguments args(args_);
    args.TextStyle(style.Object);
    if (!args._Font.IsSet()) {
        args.Font(FSlateFontInfo(
            detail::stream_font(defaults.font_path_), defaults.font_size_));
    }
    STextBlock::Construct(args);
}
