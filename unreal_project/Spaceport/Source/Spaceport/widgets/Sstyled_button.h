#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Input/SButton.h>


class Sstyled_text_block;

class Sstyled_button : public SButton
{
public:
    using FArguments = SButton::FArguments;

    void Construct(FArguments const & args);

    FText const & get_text() const;
    void set_text(FText const & text);

private:
    TSharedPtr<Sstyled_text_block> text_;
};
