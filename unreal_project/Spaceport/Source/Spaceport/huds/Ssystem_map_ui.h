#pragma once

#include "Shud_widget_base.h"

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>


class SHorizontalBox;

class Ssystem_map_ui : public Shud_widget_base
{
public:
    SLATE_BEGIN_ARGS(Ssystem_map_ui)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    void rebuild();
    void reset();

private:
    TSharedPtr<SHorizontalBox> hbox_;
};
