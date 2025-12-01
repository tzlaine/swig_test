#include "Sstyled_text_block.h"

#include <ui_defaults.h>


void Sstyled_text_block::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    STextBlock::FArguments args(args_);
    args.TextStyle(ui_defaults().TextBlock_style_.Get());
    if (!args._Font.IsSet())
        args.Font(FSlateFontInfo(defaults.font_.Get(), defaults.font_size_));
    STextBlock::Construct(args);
}
