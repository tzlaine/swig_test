#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Input/SEditableTextBox.h>


class Sstyled_editable_text_box : public SEditableTextBox
{
public:
    using FArguments = SEditableTextBox::FArguments;

    void Construct(FArguments const & args);
};
