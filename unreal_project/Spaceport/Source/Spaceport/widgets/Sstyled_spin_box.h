#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Input/SSpinBox.h>

#include <functional>


template<typename T>
class Sstyled_spin_box : public SSpinBox<T>
{
public:
    using FArguments = SSpinBox<T>::FArguments;

    void Construct(FArguments const & args_)
    {
        auto const & defaults = ui_defaults();
        detail::FObjectFinder<USlateWidgetStyleAsset> style(
            *defaults.USpinBox_style_path_);
        FArguments args(args_);
        args.SpinBoxStyle(style.Object);
        SSpinBox<T>::Construct(args);
    }
};

class Sstyled_int_spin_box : public Sstyled_spin_box<int>
{};

class Sstyled_float_spin_box : public Sstyled_spin_box<float>
{};
