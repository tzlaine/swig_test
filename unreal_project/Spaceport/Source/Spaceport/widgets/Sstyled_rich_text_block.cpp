#include "Sstyled_rich_text_block.h"

#include <ui_defaults.h>

#include <CommonUIRichTextData.h>
#include <Components/RichTextBlock.h>
#include <Engine/DataTable.h>


namespace {
    TSharedPtr<FSlateStyleSet>
        make_rich_text_styles(/* TODO FTextBlockStyle *& default_style*/)
    {
        auto const & defaults = ui_defaults();
        check(defaults.rich_text_styles_);
        check(defaults.rich_text_icons_);
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
        for (auto it =
                 defaults.rich_text_icons_->GetRowMap().CreateConstIterator();
             it;
             ++it) {
            if (auto const * row =
                    reinterpret_cast<FRichTextIconData const *>(it.Value())) {
                FInlineTextImageStyle image_style;
                row->ResourceObject.LoadSynchronous();
                image_style.Image.SetResourceObject(row->ResourceObject.Get());
                retval->Set(it.Key(), image_style);
            }
        }
        return retval;
    }
}

// TODO FTextBlockStyle * Sstyled_rich_text_block::default_style_ = nullptr;
TSharedPtr<FSlateStyleSet> Sstyled_rich_text_block::styles_;

void Sstyled_rich_text_block::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    SRichTextBlock::FArguments args(args_);

    args.DecoratorStyleSet(get_rich_text_styles());

    TArray<TSharedRef<class ITextDecorator>> decorators;
    decorators.Add(SRichTextBlock::ImageDecorator());
    args.Decorators(decorators);

    default_style_ = std::make_unique<FTextBlockStyle>(
        *ui_defaults()
             .RichTextBlock_style_->GetStyleChecked<FTextBlockStyle>());
    // TODO: Set default style as a static from the row->TextStyle called
    // "Default" (or "default"?).
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
