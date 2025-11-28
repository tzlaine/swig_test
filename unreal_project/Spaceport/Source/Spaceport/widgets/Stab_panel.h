#pragma once

#include "widgets/Sstyled_button.h"

#include <CoreMinimal.h>
#include <Slate/SCommonAnimatedSwitcher.h>
#include <Widgets/SCompoundWidget.h>


class Stab_panel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Stab_panel) {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    template<typename I, typename S>
    void insert(I first, S last)
    {
        int i = 0;
        for (; first != last; ++first, ++i) {
            auto && [button_text, widget] = *first;
            buttons_->AddSlot()[SNew(Sstyled_button)
                                    .Text(loc_text(button_text))
                                    .OnClicked_Lambda([this, i] {
                                        switcher_->TransitionToIndex(i);
                                        return FReply::Handled();
                                    })];
            switcher_->AddSlot()[widget.ToSharedRef()];
        }
    }

private:
    TSharedPtr<SHorizontalBox> buttons_;
    TSharedPtr<SCommonAnimatedSwitcher> switcher_;
};
