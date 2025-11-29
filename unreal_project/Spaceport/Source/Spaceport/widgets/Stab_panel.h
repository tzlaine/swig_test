#pragma once

#include "widgets/Sstyled_button.h"

#include <functional>

#include <CoreMinimal.h>
#include <Slate/SCommonAnimatedSwitcher.h>
#include <Widgets/SCompoundWidget.h>


class Stab_panel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Stab_panel) {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    int index() const;

    template<typename I, typename S>
    void panels(I first, S last)
    {
        int i = 0;
        for (; first != last; ++first, ++i) {
            auto && [button_text, widget] = *first;
            TSharedPtr<Sstyled_button> button;
            buttons_->AddSlot().AutoWidth().Padding(
                0, 0, 20, 0)[SAssignNew(button, Sstyled_button)
                                 .Text(loc_text(button_text))
                                 .OnClicked_Lambda([this, i] {
                                     switcher_->TransitionToIndex(i);
                                     return FReply::Handled();
                                 })];
            button->set_style_kind(Sstyled_button::style_kind::tab);
            switcher_->AddSlot()[widget.ToSharedRef()];
        }
        buttons_->AddSlot().FillWidth(1);
    }

    void panel_change_callback(std::function<void(int)> cb);

private:
    TSharedPtr<SHorizontalBox> buttons_;
    TSharedPtr<SCommonAnimatedSwitcher> switcher_;
    std::function<void(int)> panel_change_cb_;
};
