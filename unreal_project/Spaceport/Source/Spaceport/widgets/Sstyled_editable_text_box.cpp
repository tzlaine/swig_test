#include "Sstyled_editable_text_box.h"
#include <ui_defaults.h>


void Sstyled_editable_text_box::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    SEditableTextBox::FArguments args(args_);
    args.Style(defaults.EditableTextBox_style_.Get());
    if (!args._Font.IsSet())
        args.Font(FSlateFontInfo(defaults.font_.Get(), defaults.font_size_));
    SEditableTextBox::Construct(args);
}
