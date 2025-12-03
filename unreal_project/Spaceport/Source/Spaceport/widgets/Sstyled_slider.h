#pragma once

#include <Widgets/Input/SSlider.h>


class UMaterialInstanceDynamic;

class Sstyled_slider : public SSlider
{
public:
    using FArguments = SSlider::FArguments;

    void Construct(FArguments const & args);

    void value_changed(float value);

private:
    FSliderStyle style_;
    TStrongObjectPtr<UMaterialInstanceDynamic> fill_material_;
    FOnFloatValueChanged moved_delegate_;
};
