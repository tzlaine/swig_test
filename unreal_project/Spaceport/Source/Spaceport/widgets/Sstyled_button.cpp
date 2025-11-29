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
        rebuild_text(args._Text.Get());
}

FText const & Sstyled_button::get_text() const
{
    check(text_);
    return text_->GetText();
}

void Sstyled_button::set_text(FText const & text)
{
    check(text_);
    text_->SetText(text);
}

void Sstyled_button::set_style_kind(style_kind kind)
{
    if (kind == style_kind_)
        return;
    style_kind_ = kind;
    rebuild_text(text_->GetText());
}

void Sstyled_button::selected_tab(bool selected)
{
    check(style_kind_ == style_kind::tab);
    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UButton_style_path_);
    SButton::FArguments args;
    args.ButtonStyle(style.Object);
    if (selected)
        SetColorAndOpacity(selected_color_);
    else
        SetColorAndOpacity(FLinearColor(1, 1, 1));
}

void Sstyled_button::set_selected_color(FColor selected_color)
{
    selected_color_ = selected_color;
}

void Sstyled_button::rebuild_text(FText const & text)
{
    auto text_widget = SAssignNew(text_, Sstyled_text_block).Text(text);
    if (style_kind_ == style_kind::tab) {
        text_->SetJustification(ETextJustify::Left);
        auto with_padding = SNew(SBox).HAlign(HAlign_Left)[text_widget];
        SButton::SetContent(with_padding);
    } else {
        auto with_padding =
            SNew(SBox).Padding(10.0f).HAlign(HAlign_Center)[text_widget];
        SButton::SetContent(with_padding);
    }
}
