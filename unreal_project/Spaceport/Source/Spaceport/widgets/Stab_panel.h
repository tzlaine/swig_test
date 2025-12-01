#pragma once

#include "widgets/Sstyled_button.h"

#include <functional>

#include <CoreMinimal.h>
#include <Slate/SCommonAnimatedSwitcher.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/SCompoundWidget.h>


class Stab_panel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Stab_panel) {}
    SLATE_ARGUMENT(FColor, selected_tab_color)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    int index() const;

    template<typename I, typename S>
    void panels(I first, S last)
    {
        check(buttons_.empty());
        int i = 0;
        for (; first != last; ++first, ++i) {
            auto && [button_text, widget] = *first;
            TSharedPtr<Sstyled_button> button;
            buttons_hbox_->AddSlot().AutoWidth().Padding(
                0, 0, 20, 0)[SAssignNew(button, Sstyled_button)
                                 .Text(loc_text(button_text))
                                 .OnClicked_Lambda([this, i] {
                                     int const prev_i =
                                         switcher_->GetActiveWidgetIndex();
                                     buttons_[prev_i]->selected_tab(false);
                                     buttons_[i]->selected_tab(true);
                                     switcher_->TransitionToIndex(i);
                                     return FReply::Handled();
                                 })];
            button->set_style_kind(Sstyled_button::style_kind::tab);
            button->set_selected_color(selected_tab_color_);
            buttons_.push_back(button);
            switcher_->AddSlot()[widget.ToSharedRef()];
        }
        buttons_hbox_->AddSlot().FillWidth(1);
        buttons_.front()->selected_tab(true);
    }

    void panel_change_callback(std::function<void(int)> cb);

private:
    FColor selected_tab_color_;
    std::vector<TSharedPtr<Sstyled_button>> buttons_;
    TSharedPtr<SHorizontalBox> buttons_hbox_;
    TSharedPtr<SCommonAnimatedSwitcher> switcher_;
    std::function<void(int)> panel_change_cb_;
};
