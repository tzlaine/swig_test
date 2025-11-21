#include "Sstyled_editable_text_box.h"


void Sstyled_editable_text_box::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UEditableTextBox_style_path_);
    SEditableTextBox::FArguments args(args_);
    args.Style(style.Object);
    if (!args._Font.IsSet()) {
        args.Font(
            FSlateFontInfo(detail::stream_default_font(), defaults.font_size_));
    }
    SEditableTextBox::Construct(args);
}
