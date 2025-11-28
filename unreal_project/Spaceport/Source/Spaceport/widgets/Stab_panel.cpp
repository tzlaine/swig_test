#include "Stab_panel.h"

#include <SlateOptMacros.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Stab_panel::Construct(FArguments const &)
{
    // clang-format off
    ChildSlot[
        SNew(SVerticalBox)

        +SVerticalBox::Slot().AutoHeight()[
            SAssignNew(buttons_, SHorizontalBox)
        ]

        +SVerticalBox::Slot().FillHeight(1)[
            SAssignNew(switcher_, SCommonAnimatedSwitcher)
            .TransitionType(ECommonSwitcherTransition::Horizontal)
            .OnActiveIndexChanged_Lambda([this](int i) {
                if (panel_change_cb_)
                    panel_change_cb_(i);
            })
        ]
    ];
    // clang-format on
}

int Stab_panel::index() const { return switcher_->GetActiveWidgetIndex(); }

void Stab_panel::panel_change_callback(std::function<void(int)> cb)
{
    panel_change_cb_ = std::move(cb);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
