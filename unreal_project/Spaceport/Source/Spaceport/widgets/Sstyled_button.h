#pragma once

#include <Widgets/Input/SButton.h>


class Sstyled_text_block;

class Sstyled_button : public SButton
{
public:
    enum struct style_kind { typical, tab };

    using FArguments = SButton::FArguments;

    void Construct(FArguments const & args);

    FText const & get_text() const;

    void set_text(FText const & text, FSlateFontInfo * font_info = nullptr);
    void set_style_kind(style_kind kind);
    void selected_tab(bool selected);
    void set_selected_color(FColor selected_color);

private:
    void rebuild_text(FText const & text, FSlateFontInfo * font_info);

    TSharedPtr<Sstyled_text_block> text_;
    style_kind style_kind_ = style_kind::typical;
    FColor selected_color_;
};
