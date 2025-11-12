#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Notifications/SProgressBar.h>


class Sstyled_progress_bar : public SProgressBar
{
public:
    using FArguments = SProgressBar::FArguments;

    void Construct(FArguments const & args);
};
