#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Text/STextBlock.h>


class Sstyled_text_block : public STextBlock
{
public:
    using FArguments = STextBlock::FArguments;

    void Construct(FArguments const & args);
};
