#include "Sstyled_rich_text_block.h"

#include "icon_data.h"
#include <ui_defaults.h>

#include <CommonUIRichTextData.h>
#include <Components/RichTextBlock.h>
#include <Engine/DataTable.h>


namespace {
    TSharedPtr<FSlateStyleSet>
    make_rich_text_styles(FTextBlockStyle const *& default_style)
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
                if (it.Key() == FName(TEXT("Default")))
                    default_style = &row->TextStyle;
            }
        }
        for (auto it =
                 defaults.rich_text_icons_->GetRowMap().CreateConstIterator();
             it;
             ++it) {
            if (auto const * row =
                    reinterpret_cast<Ficon_data const *>(it.Value())) {
                FInlineTextImageStyle image_style;
                row->ResourceObject.LoadSynchronous();
                image_style.Image.SetResourceObject(row->ResourceObject.Get());
                image_style.Image.TintColor = row->color_and_opacity_;
                image_style.Image.ImageSize = row->ImageSize;
                retval->Set(it.Key(), image_style);
            }
        }
        return retval;
    }
}

FTextBlockStyle const * Sstyled_rich_text_block::default_style_ = nullptr;
TSharedPtr<FSlateStyleSet> Sstyled_rich_text_block::styles_;

void Sstyled_rich_text_block::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    SRichTextBlock::FArguments args(args_);

    args.DecoratorStyleSet(get_rich_text_styles());

    TArray<TSharedRef<class ITextDecorator>> decorators;
    decorators.Add(SRichTextBlock::ImageDecorator());
    args.Decorators(decorators);

    args.TextStyle(default_style_);

    SRichTextBlock::Construct(args);
}

FSlateStyleSet const * Sstyled_rich_text_block::get_rich_text_styles()
{
    if (!styles_)
        styles_ = make_rich_text_styles(default_style_);
    return styles_.Get();
}
