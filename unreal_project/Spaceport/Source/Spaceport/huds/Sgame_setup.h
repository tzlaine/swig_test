#pragma once

#include "game_data_t.hpp"

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SBoxPanel.h>


class Sstyled_int_spin_box;

class Sgame_setup : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Sgame_setup)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

private:
    game_start_params_t params_ = default_game_start_params();
};
