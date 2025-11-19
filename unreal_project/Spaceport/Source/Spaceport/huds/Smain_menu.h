#pragma once

#include "Shud_widget_base.h"
#include "animations.hpp"
#include "widgets/Sstyled_button.h"

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SBoxPanel.h>


class Smain_menu : public Shud_widget_base
{
public:
    Smain_menu();

    SLATE_BEGIN_ARGS(Smain_menu)
        {}
    SLATE_ARGUMENT(bool, in_game)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    void Tick(FGeometry const & g, double t, float dt) override;

    void rebuild();
    void in_game(bool b);
    void have_saves(bool b);

    bool cancelable() override;
    void show(UWorld * w) override;

private:
    TSharedPtr<SVerticalBox> vbox_;

    TSharedPtr<Sstyled_button> continue_bn_;
    TSharedPtr<Sstyled_button> load_game_bn_;

    animations anims_;

    bool in_game_ = true;
};
