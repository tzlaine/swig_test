#include "Sstyled_slider.h"

#include "materials.h"
#include <ui_defaults.h>

#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>


void Sstyled_slider::Construct(FArguments const & args_)
{
    auto const & defaults = ui_defaults();
    float const value = args_._Value.IsSet() ? args_._Value.Get() : 1.0f;

    // Use most of the style info set in the BP.
    style_ = defaults.Slider_style_;

    UMaterialInterface * base_material = materials().slider_fill_;
    fill_material_.Reset(
        UMaterialInstanceDynamic::Create(base_material, nullptr));
    fill_material_->SetScalarParameterValue(TEXT("value"), value);
    style_.NormalBarImage.SetResourceObject(fill_material_.Get());
    style_.HoveredBarImage.SetResourceObject(fill_material_.Get());
    style_.DisabledBarImage.SetResourceObject(fill_material_.Get());

    SSlider::FArguments args(args_);
    args.Style(&style_);
    moved_delegate_ = std::move(args._OnValueChanged);
    args._OnValueChanged.BindLambda(
        [this](float value) { value_changed(value); });
    SSlider::Construct(args);
}

void Sstyled_slider::value_changed(float value)
{
    fill_material_->SetScalarParameterValue(TEXT("value"), value);
    moved_delegate_.Execute(value);
}
