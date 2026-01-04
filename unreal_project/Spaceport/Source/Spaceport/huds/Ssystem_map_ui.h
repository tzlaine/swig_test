#pragma once

#include "Shud_widget_base.h"

#include "constants.hpp"

#include <chrono>

#include <CoreMinimal.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Widgets/SCompoundWidget.h>


class SHorizontalBox;
class SButton;
class UMaterialInstanceDynamic;

struct button_materials_and_style
{
    TStrongObjectPtr<UMaterialInstanceDynamic> normal_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> hovered_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> pressed_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> disabled_material_;
    FSlateBrush normal_brush_;
    FSlateBrush hovered_brush_;
    FSlateBrush pressed_brush_;
    FSlateBrush disabled_brush_;
    FButtonStyle style_;
};

class Ssystem_map_ui : public Shud_widget_base
{
public:
    SLATE_BEGIN_ARGS(Ssystem_map_ui)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    void rebuild(int system_id);
    void play_speed(int speed);
    void paused(bool b);
    void reset();

private:
    std::vector<FButtonStyle> button_styles_;
    TSharedPtr<SHorizontalBox> hbox_;

    FSlateBrush speed_pip_brush_;
    TStrongObjectPtr<UMaterialInstanceDynamic> speed_pip_material_;

    button_materials_and_style slower_speed_style_;
    button_materials_and_style play_style_;
    button_materials_and_style pause_style_;
    button_materials_and_style faster_speed_style_;
#if 0
    FSlateBrush slower_speed_brush_;
    FSlateBrush slower_speed_hovered_brush_;
    FSlateBrush slower_speed_pressed_brush_;
    FSlateBrush play_brush_;
    FSlateBrush play_hovered_brush_;
    FSlateBrush play_pressed_brush_;
    FSlateBrush pause_brush_;
    FSlateBrush pause_hovered_brush_;
    FSlateBrush pause_pressed_brush_;
    FSlateBrush faster_speed_brush_;
    FSlateBrush faster_speed_hovered_brush_;
    FSlateBrush faster_speed_pressed_brush_;
    FButtonStyle slower_button_style_;
    FButtonStyle play_button_style_;
    FButtonStyle pause_button_style_;
    FButtonStyle faster_button_style_;
    TStrongObjectPtr<UMaterialInstanceDynamic> slower_speed_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> slower_speed_hovered_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> slower_speed_pressed_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> play_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> play_hovered_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> play_pressed_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> pause_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> pause_hovered_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> pause_pressed_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> faster_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> faster_hovered_material_;
    TStrongObjectPtr<UMaterialInstanceDynamic> faster_pressed_material_;
#endif
    TSharedPtr<SButton> slower_speed_button_;
    TSharedPtr<SButton> play_pause_button_;
    TSharedPtr<SButton> faster_speed_button_;

    std::chrono::time_point<std::chrono::system_clock> prev_click_time_;
    int prev_click_object_ = object_none;
    int system_id_ = system_none;
};
