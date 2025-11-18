#pragma once

#include "Shud_widget_base.h"
#include "model.hpp"

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SBoxPanel.h>


class Sstyled_int_spin_box;

class Sgame_setup : public Shud_widget_base
{
public:
    SLATE_BEGIN_ARGS(Sgame_setup)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    bool cancelable() override;
    void cancel(UWorld * w) override;

private:
    void back();

    game_start_params_t params_ = default_game_start_params();
};
