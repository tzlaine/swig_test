#include "Stab_panel.h"

#include <SlateOptMacros.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Stab_panel::Construct(FArguments const &)
{
    // clang-format off
    ChildSlot[
        SNew(SVerticalBox)

        +SVerticalBox::Slot()[
            SAssignNew(buttons_, SHorizontalBox)
        ]

        +SVerticalBox::Slot()[
            SAssignNew(switcher_, SCommonAnimatedSwitcher)
        ]
    ];
    // clang-format on
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
