#pragma once

#include <Widgets/Layout/SBorder.h>


class Sstyled_border : public SBorder
{
public:
    using FArguments = SBorder::FArguments;

    void Construct(FArguments const & args);
};
