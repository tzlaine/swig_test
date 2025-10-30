#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Input/SButton.h>

#include <functional>


class Sstyled_button : public SButton
{
public:
    using FArguments = SButton::FArguments;

    void Construct(FArguments const & args);
};
