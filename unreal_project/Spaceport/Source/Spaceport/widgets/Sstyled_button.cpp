#include "Sstyled_button.h"


void Sstyled_button::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    detail::FObjectFinder<USlateWidgetStyleAsset> style(
        *defaults.UButton_style_path_);
    SButton::FArguments args(args_);
    args.ButtonStyle(style.Object);
    args.OnClicked(this, &Sstyled_button::trampoline);
    SButton::Construct(args);
}

FReply Sstyled_button::trampoline()
{
    if (fn_)
        fn_();
    return FReply::Handled();
}
