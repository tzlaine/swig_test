#pragma once

#include <ui_defaults.h>

#include <Widgets/Input/SSpinBox.h>

#include <functional>


template<typename T, int FontScalePct = 100>
class Sstyled_spin_box : public SSpinBox<T>
{
public:
    using FArguments = SSpinBox<T>::FArguments;

    void Construct(FArguments const & args_)
    {
        auto const & defaults = ui_defaults();
        auto args(args_);
        args.Style(defaults.SpinBox_style_.Get());
        args.Font(FSlateFontInfo(
            defaults.font_.Get(), defaults.font_size_ * FontScalePct / 100.0));
        SSpinBox<T>::Construct(args);
    }
};

class Sstyled_int_spin_box : public Sstyled_spin_box<int>
{};

class Sstyled_float_spin_box : public Sstyled_spin_box<float>
{};
