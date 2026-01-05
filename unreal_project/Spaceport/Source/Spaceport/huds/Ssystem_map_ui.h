#pragma once

#include "Shud_widget_base.h"

#include "constants.hpp"
#include "game_data.hpp"

#include <chrono>

#include <CoreMinimal.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Widgets/SCompoundWidget.h>


class SHorizontalBox;
class SButton;
class SScrollBox;
class Sstyled_text_block;
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
    void date(date_t const & d);
    void reset();

private:
    std::vector<FButtonStyle> button_styles_;
    TSharedPtr<SHorizontalBox> hbox_;

    FSlateBrush speed_pip_brush_;
    TStrongObjectPtr<UMaterialInstanceDynamic> speed_pip_material_;

    date_t date_ = {2, 2, 2};
    TSharedPtr<Sstyled_text_block> date_text_;

    button_materials_and_style slower_speed_style_;
    button_materials_and_style play_style_;
    button_materials_and_style pause_style_;
    button_materials_and_style faster_speed_style_;
    TSharedPtr<SButton> slower_speed_button_;
    TSharedPtr<SButton> play_pause_button_;
    TSharedPtr<SButton> faster_speed_button_;

    TSharedPtr<SScrollBox> sidepanel_scrollbox_;

    std::chrono::time_point<std::chrono::system_clock> prev_click_time_;
    int prev_click_object_ = object_none;
    int system_id_ = system_none;
};
