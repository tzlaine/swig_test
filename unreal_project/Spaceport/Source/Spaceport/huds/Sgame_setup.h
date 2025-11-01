#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SBoxPanel.h>


class Sgame_setup : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Sgame_setup)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);
};
