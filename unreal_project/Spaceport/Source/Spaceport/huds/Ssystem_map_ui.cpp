#include "Ssystem_map_ui.h"
#include "Agame_state.h"
#include "Aplayer_controller.h"
#include "Arender_target.h"
#include "materials.h"
#include "model_util.hpp"
#include "utility.hpp"
#include "game_data_formatters.hpp"
#include "widgets/Sstyled_text_block.h"
#include <ui_defaults.h>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Materials/MaterialInterface.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/Layout/SScaleBox.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

int const object_render_size = 150; // TODO
int const side_panel_width = 200;   // TODO

namespace {
    void speed_button_material(
        UMaterialInterface * base_material, button_materials_and_style & bmas)
    {
        bmas.normal_material_.Reset(
            UMaterialInstanceDynamic::Create(base_material, nullptr));
        bmas.normal_material_->SetVectorParameterValue(
            TEXT("color"), ui_defaults().speed_button_normal_color_);
        bmas.normal_brush_.SetResourceObject(bmas.normal_material_.Get());

        bmas.hovered_material_.Reset(
            UMaterialInstanceDynamic::Create(base_material, nullptr));
        bmas.hovered_material_->SetVectorParameterValue(
            TEXT("color"), ui_defaults().speed_button_hovered_color_);
        bmas.hovered_brush_.SetResourceObject(bmas.hovered_material_.Get());

        bmas.pressed_material_.Reset(
            UMaterialInstanceDynamic::Create(base_material, nullptr));
        bmas.pressed_material_->SetVectorParameterValue(
            TEXT("color"), ui_defaults().speed_button_pressed_color_);
        bmas.pressed_brush_.SetResourceObject(bmas.pressed_material_.Get());

        bmas.disabled_material_.Reset(
            UMaterialInstanceDynamic::Create(base_material, nullptr));
        bmas.disabled_material_->SetVectorParameterValue(
            TEXT("color"), ui_defaults().speed_button_disabled_color_);
        bmas.disabled_brush_.SetResourceObject(bmas.disabled_material_.Get());

        bmas.style_.Normal = bmas.normal_brush_;
        bmas.style_.Hovered = bmas.hovered_brush_;
        bmas.style_.Pressed = bmas.pressed_brush_;
        bmas.style_.Disabled = bmas.disabled_brush_;
    }
}

void Ssystem_map_ui::Construct(FArguments const & args)
{
    {
        UMaterialInterface * base_material = materials().rotator_pip_;
        speed_pip_material_.Reset(
            UMaterialInstanceDynamic::Create(base_material, nullptr));
        speed_pip_material_->SetScalarParameterValue(TEXT("num_pips"), 5);
        speed_pip_material_->SetScalarParameterValue(TEXT("curr_pip"), 0);
        speed_pip_brush_.SetResourceObject(speed_pip_material_.Get());
    }

    speed_button_material(
        materials().slower_speed_button_, slower_speed_style_);
    speed_button_material(materials().play_button_, play_style_);
    speed_button_material(materials().pause_button_, pause_style_);
    speed_button_material(
        materials().faster_speed_button_, faster_speed_style_);

    ChildSlot
        [SNew(SOverlay) +
         SOverlay::Slot()
             .HAlign(HAlign_Right)
             .VAlign(VAlign_Top)
             .Padding(0, 5)
                 [SNew(SBox)
                      .WidthOverride(side_panel_width)
                      .HeightOverride(90)
                          [SNew(SVerticalBox) +
                           SVerticalBox::Slot()
                               .HAlign(HAlign_Center)
                               .VAlign(VAlign_Center)
                               .Padding(0, 0, 0, 5)
                               .FillHeight(
                                   1)[SAssignNew(date_text_, Sstyled_text_block)
                                          .Text(to_ftext(date_))
                                          .Font(FSlateFontInfo(
                                              ui_defaults().font_.Get(),
                                              ui_defaults().font_size_ / 2))] +
                           SVerticalBox::Slot()
                               .VAlign(VAlign_Center)
                               .Padding(0, 0, 0, 5)
                               .FillHeight(2)
                                   [SNew(SHorizontalBox) +
                                    SHorizontalBox::Slot().AutoWidth()
                                        [SAssignNew(
                                             slower_speed_button_, SButton)
                                             .ButtonStyle(
                                                 &slower_speed_style_.style_)
                                             .OnClicked_Lambda([] {
                                                 player_controller()
                                                     ->decrease_play_speed();
                                                 return FReply::Handled();
                                             })] +
                                    SHorizontalBox::Slot().FillWidth(1) +
                                    SHorizontalBox::Slot().AutoWidth()
                                        [SAssignNew(play_pause_button_, SButton)
                                             .ButtonStyle(&play_style_.style_)
                                             .OnClicked_Lambda([] {
                                                 player_controller()
                                                     ->server_toggle_pause();
                                                 return FReply::Handled();
                                             })] +
                                    SHorizontalBox::Slot().FillWidth(1) +
                                    SHorizontalBox::Slot().AutoWidth()
                                        [SAssignNew(
                                             faster_speed_button_, SButton)
                                             .ButtonStyle(
                                                 &faster_speed_style_.style_)
                                             .OnClicked_Lambda([] {
                                                 player_controller()
                                                     ->increase_play_speed();
                                                 return FReply::Handled();
                                             })]] +
                           SVerticalBox ::Slot().FillHeight(
                               1)[SNew(SImage).Image(&speed_pip_brush_)]]] +
         SOverlay::Slot()
             .HAlign(HAlign_Fill)
             .VAlign(VAlign_Bottom)
             .Padding(0, 10)[SAssignNew(hbox_, SHorizontalBox)]];

    auto * gs = ::world()->GetGameState<Agame_state>();
    if (!gs)
        return;
    paused(gs->play_state_ == play_state::paused);
    play_speed(gs->play_speed_);
}

void Ssystem_map_ui::rebuild(int system_id)
{
    reset();
    system_id_ = system_id;

    auto * pc = player_controller();
    check(pc);
    std::span<Arender_target const * const> object_renders =
        pc->system_map_object_renders();
    auto system = ::system(pc->gs(), system_id);
    check(system);

    button_styles_.resize(object_renders.size());

    for (int i = 0, last = (int)object_renders.size(); i < last; ++i) {
        auto const rt = object_renders[i];
        int const planet_id =
            i == 0 ? -system_id : system->first_planet + (i - 1);

        auto const handle_double_click = [pc, i, this] {
            pc->zoom_to_system_object(i);
        };

        auto const handle_click = [planet_id, handle_double_click, this] {
            if (double_clicked(
                    prev_click_object_,
                    prev_click_time_,
                    planet_id,
                    object_none)) {
                handle_double_click();
                return FReply::Handled();
            }

            auto * pc = player_controller();
            check(pc);

            if (planet_id < 0) {
                int const system_id = -planet_id;
                auto system = ::system(pc->gs(), system_id);
                check(system);
                // TODO: Show info on system->star in the left-side panel.
            } else {
                auto planet = ::planet(pc->gs(), planet_id);
                check(planet);
                // TODO: Show info on planet in the left-side panel.
            }

            return FReply::Handled();
        };

        FButtonStyle & style = button_styles_[i];
        style.Normal = rt->brush();
        style.Hovered = rt->brush();
        style.Pressed = rt->brush();

        hbox_->AddSlot().AutoWidth().Padding(
            5, 0)[SNew(SBox)
                      .WidthOverride(object_render_size)
                      .HeightOverride(object_render_size)
                          [SNew(SButton).ButtonStyle(&style).OnClicked_Lambda(
                              handle_click)]];
        // TODO .ToolTipText(FText::FromString(TEXT("TODO")))
    }
}

void Ssystem_map_ui::play_speed(int speed)
{
    if (!speed_pip_material_)
        return;
    speed_pip_material_->SetScalarParameterValue(TEXT("curr_pip"), speed - 1);
    slower_speed_button_->SetEnabled(speed != 1);
    faster_speed_button_->SetEnabled(speed != 5);
}

void Ssystem_map_ui::paused(bool b)
{
    if (!play_pause_button_)
        return;
    play_pause_button_->SetButtonStyle(
        b ? &play_style_.style_ : &pause_style_.style_);
}

void Ssystem_map_ui::date(date_t const & d)
{
    date_ = d;
    if (!date_text_)
        return;
    date_text_->SetText(to_ftext(date_));
}

void Ssystem_map_ui::reset()
{
    system_id_ = -1;
    hbox_->ClearChildren();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
