#pragma once

#include "Shud_widget_base.h"

#include <CoreMinimal.h>


class Sstyled_button;
class SCommonAnimatedSwitcher;

class Soptions : public Shud_widget_base
{
public:
    SLATE_BEGIN_ARGS(Soptions)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    bool cancelable() override;

private:
    TSharedPtr<SCommonAnimatedSwitcher> switcher_;
};
