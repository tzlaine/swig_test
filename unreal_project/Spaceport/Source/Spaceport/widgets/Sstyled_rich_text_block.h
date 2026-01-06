#pragma once

#include <memory>

#include <Widgets/Text/SRichTextBlock.h>


class FSlateStyleSet;

class Sstyled_rich_text_block : public SRichTextBlock
{
public:
    using FArguments = SRichTextBlock::FArguments;

    void Construct(FArguments const & args);

private:
    static FSlateStyleSet const * get_rich_text_styles();

    std::unique_ptr<FTextBlockStyle> default_style_;

    static TSharedPtr<FSlateStyleSet> styles_;
};
