#pragma once

#include <Widgets/Input/SCheckBox.h>


class Sstyled_check_box : public SCheckBox
{
public:
    using FArguments = SCheckBox::FArguments;

    void Construct(FArguments const & args);
};
