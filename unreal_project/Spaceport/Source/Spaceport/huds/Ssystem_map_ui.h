#pragma once

#include "Shud_widget_base.h"

#include "constants.hpp"

#include <chrono>

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

    void rebuild(int system_id);
    void reset();

private:
    std::vector<FButtonStyle> button_styles_;
    TSharedPtr<SHorizontalBox> hbox_;
    std::chrono::time_point<std::chrono::system_clock> prev_click_time_;
    int prev_click_object_ = object_none;
    int system_id_ = system_none;
};
