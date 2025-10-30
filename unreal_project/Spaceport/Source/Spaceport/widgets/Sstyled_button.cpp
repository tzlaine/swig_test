#include "Sstyled_button.h"
#include "Sstyled_text_block.h"


void Sstyled_button::Construct(FArguments const & args_)
{
    bool const has_text = args_._Text.IsSet();

    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UButton_style_path_);
    SButton::FArguments args(args_);
    args.ButtonStyle(style.Object);
    if (has_text) {
        args.ContentPadding(FMargin(4, 2));
        args.HAlign(HAlign_Center);
        args.VAlign(VAlign_Center);
    }
    SButton::Construct(args);

    if (has_text)
        SButton::SetContent(SNew(Sstyled_text_block).Text(args._Text));
}
