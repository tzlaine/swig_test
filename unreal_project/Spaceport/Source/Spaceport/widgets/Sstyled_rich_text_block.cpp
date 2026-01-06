#include "Sstyled_rich_text_block.h"

#include <ui_defaults.h>

#include <Components/RichTextBlock.h>
#include <Engine/DataTable.h>


namespace {
    TSharedPtr<FSlateStyleSet> make_rich_text_styles()
    {
        auto const & defaults = ui_defaults();
        check(defaults.rich_text_styles_);
        TSharedPtr<FSlateStyleSet> retval(
            new FSlateStyleSet("rich_text_styles"));
        for (auto it =
                 defaults.rich_text_styles_->GetRowMap().CreateConstIterator();
             it;
             ++it) {
            if (auto const * row =
                    reinterpret_cast<FRichTextStyleRow const *>(it.Value())) {
                retval->Set(it.Key(), row->TextStyle);
            }
        }
        return retval;
    }
}

TSharedPtr<FSlateStyleSet> Sstyled_rich_text_block::styles_;

void Sstyled_rich_text_block::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    SRichTextBlock::FArguments args(args_);
    args.DecoratorStyleSet(get_rich_text_styles());
    default_style_ = std::make_unique<FTextBlockStyle>(
        *ui_defaults()
             .RichTextBlock_style_->GetStyleChecked<FTextBlockStyle>());
    default_style_->SetFont(
        FSlateFontInfo(defaults.font_.Get(), defaults.rich_text_font_size_));
    args.TextStyle(default_style_.get());
    SRichTextBlock::Construct(args);
}

FSlateStyleSet const * Sstyled_rich_text_block::get_rich_text_styles()
{
    if (!styles_)
        styles_ = make_rich_text_styles();
    return styles_.Get();
}
