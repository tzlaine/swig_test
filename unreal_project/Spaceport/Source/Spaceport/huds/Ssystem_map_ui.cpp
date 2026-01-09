#include "Ssystem_map_ui.h"
#include "Agame_state.h"
#include "Aplayer_controller.h"
#include "Arender_target.h"
#include "materials.h"
#include "model_util.hpp"
#include "utility.hpp"
#include "game_data_formatters.hpp"
#include "game_data_metadata.hpp"
#include "widgets/Sstyled_scroll_box.h"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Sstyled_rich_text_block.h"
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
int const side_panel_width = 450;   // TODO
int const padding = 5;

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

    FText scientific_notation(double x)
    {
        int const exp = (int)std::floor(std::log10(double(x)));
        double const largest_pow_10_before = std::pow(10.0, exp);
        double const mantissa = x / largest_pow_10_before;
        FNumberFormattingOptions options;
        options.MaximumFractionalDigits = 4;
        return FText::Format(
            ::loc_text(TEXT("scientific_notation")),
            FText::AsNumber(mantissa, &options),
            FText::AsNumber(exp));
    }

    template<typename T, typename Meta>
    FText sidepanel_value(
        T const & x, Meta meta, std::string_view prefix, bool percentage)
    {
        std::string field_name(prefix);
        field_name += "value_";
        if constexpr (std::is_enum_v<T>) {
            FText x_as_text = ::loc_text(std::format("{}{}", field_name, x));
            field_name += meta.name_;
            return FText::Format(::loc_text(field_name), x_as_text);
        } else if constexpr (std::is_floating_point_v<T>) {
            field_name += meta.name_;
            FNumberFormattingOptions options;
            if (percentage) {
                options.MaximumFractionalDigits = 1;
                return FText::Format(
                    ::loc_text(field_name), FText::AsPercent(x, &options));
            } else if (10e6 < x) {
                return FText::Format(
                    ::loc_text(field_name), scientific_notation(x));
            } else {
                options.MaximumFractionalDigits = 2;
                return FText::Format(
                    ::loc_text(field_name), FText::AsNumber(x, &options));
            }
        } else {
            static_assert(std::is_integral_v<T>);
            field_name += meta.name_;
            return FText::Format(::loc_text(field_name), FText::AsNumber(x));
        }
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

    int const speed_panel_height = 100;
    int const planets_panel_height = object_render_size + padding;

    ChildSlot
        [SNew(SOverlay) +
         SOverlay::Slot()
             .HAlign(HAlign_Right)
             .VAlign(VAlign_Top)
             .Padding(0, padding)
                 [SNew(SBox)
                      .WidthOverride(side_panel_width)
                      .HeightOverride(speed_panel_height)
                          [SNew(SVerticalBox) +
                           SVerticalBox::Slot()
                               .HAlign(HAlign_Center)
                               .VAlign(VAlign_Center)
                               .Padding(0, 0, 0, padding)
                               .FillHeight(1)
                                   [SAssignNew(date_text_, Sstyled_text_block)
                                        .Text(to_ftext(date_))
                                        .Font(FSlateFontInfo(
                                            ui_defaults().font_.Get(),
                                            ui_defaults().font_size_ * 0.75))] +
                           SVerticalBox::Slot()
                               .VAlign(VAlign_Center)
                               .Padding(0, 0, 0, padding)
                               .FillHeight(1)
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
                                             })
                                             .IsFocusable(false)] +
                                    SHorizontalBox::Slot().FillWidth(1) +
                                    SHorizontalBox::Slot().AutoWidth()
                                        [SAssignNew(play_pause_button_, SButton)
                                             .ButtonStyle(&play_style_.style_)
                                             .OnClicked_Lambda([] {
                                                 player_controller()
                                                     ->server_toggle_pause();
                                                 return FReply::Handled();
                                             })
                                             .IsFocusable(false)] +
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
                                             })
                                             .IsFocusable(false)]] +
                           SVerticalBox::Slot().FillHeight(
                               1)[SNew(SImage).Image(&speed_pip_brush_)]]] +

         SOverlay::Slot()
             .HAlign(HAlign_Right)
             .VAlign(VAlign_Fill)
             .Padding(
                 0,
                 speed_panel_height + padding,
                 0,
                 planets_panel_height +
                     padding)[ASSIGN_STYLED_SCROLL_BOX(sidepanel_scrollbox_)] +

         SOverlay::Slot()
             .HAlign(HAlign_Fill)
             .VAlign(VAlign_Bottom)
             .Padding(0, padding)[SAssignNew(hbox_, SHorizontalBox)]];

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

        auto const add_panel_detail = [this](
                                          auto const & x,
                                          auto meta,
                                          std::string_view field_prefix,
                                          bool percentage = false) {
            std::string field_name(field_prefix);
            field_name += meta.name_;
            sidepanel_scrollbox_->AddSlot()
                .HAlign(HAlign_Fill)
                .Padding(0, padding, 0, padding)
                    [SNew(SBox).WidthOverride(side_panel_width)
                         [SNew(SHorizontalBox) +
                          SHorizontalBox::Slot().FillWidth(
                              50)[SNew(Sstyled_rich_text_block)
                                      .Text(::loc_text(field_name))] +
                          SHorizontalBox::Slot().FillWidth(
                              50)[SNew(Sstyled_rich_text_block)
                                      .Text(sidepanel_value(
                                          x.*meta.ptr_,
                                          meta,
                                          field_prefix,
                                          percentage))
                                      .Justification(ETextJustify::Right)]]];
        };

        auto const handle_click = [=, this] {
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

            auto system = ::system(pc->gs(), system_id_);
            check(system);

            sidepanel_scrollbox_->ClearChildren();

            std::string object_name = system->name.c_str();
            if (0 < i) {
                object_name += ' ';
                object_name += roman_number(i);
            }
            sidepanel_scrollbox_->AddSlot()
                .HAlign(HAlign_Fill)
                .Padding(0, padding, 0, padding)[SNew(SBox).WidthOverride(
                    side_panel_width)[SNew(Sstyled_rich_text_block)
                                          .Text(FText::FromString(
                                              FString(object_name.c_str())))
                                          .Justification(
                                              ETextJustify::Center)]];

            if (planet_id < 0) {
                sidepanel_scrollbox_->AddSlot().Padding(0, 0, 0, 2 * padding);

                add_panel_detail(
                    system->star,
                    detail::metadata<star_t>::star_class(),
                    "star_details_");
                add_panel_detail(
                    system->star,
                    detail::metadata<star_t>::temperature_k(),
                    "star_details_");
                add_panel_detail(
                    system->star,
                    detail::metadata<star_t>::solar_masses(),
                    "star_details_");
                add_panel_detail(
                    system->star,
                    detail::metadata<star_t>::solar_luminosities(),
                    "star_details_");
                add_panel_detail(
                    system->star,
                    detail::metadata<star_t>::solar_radii(),
                    "star_details_");
            } else {
                auto planet = ::planet(pc->gs(), planet_id);
                check(planet);

                FText short_desc = FText::Format(
                    ::loc_text(
                        planet->planet_type == planet_type_t::rocky
                            ? "sidepanel_rocky_planet_short_desc"
                            : "sidepanel_nonrocky_planet_short_desc"),
                    sidepanel_value(
                        planet->planet_type,
                        detail::metadata<planet_t>::planet_type(),
                        "planet_details_",
                        false),
                    sidepanel_value(
                        planet->atmosphere_type,
                        detail::metadata<planet_t>::atmosphere_type(),
                        "planet_details_",
                        false));
                sidepanel_scrollbox_->AddSlot()
                    .HAlign(HAlign_Fill)
                    .Padding(0, padding, 0, padding)[SNew(SBox).WidthOverride(
                        side_panel_width)[SNew(Sstyled_rich_text_block)
                                              .Text(std::move(short_desc))
                                              .Justification(
                                                  ETextJustify::Center)]];

                sidepanel_scrollbox_->AddSlot().Padding(0, 0, 0, 2 * padding);

                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::mass_kg(),
                    "planet_details_");
                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::radius_km(),
                    "planet_details_");
                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::gravity_g(),
                    "planet_details_");
                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::orbit_au(),
                    "planet_details_");
                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::orbital_period_y(),
                    "planet_details_");
                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::axial_tilt_d(),
                    "planet_details_");
                add_panel_detail(
                    *planet,
                    detail::metadata<planet_t>::day_h(),
                    "planet_details_");
                if (planet->planet_type == planet_type_t::rocky) {
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::surface_temperature_k(),
                        "planet_details_");
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::magnetosphere_strength(),
                        "planet_details_");
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::o2_co2_suitability(),
                        "planet_details_",
                        true);
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::atmospheric_pressure(),
                        "planet_details_");
                    if (planet->atmosphere_type ==
                        atmosphere_type_t::oxidized_type_b) {
                        add_panel_detail(
                            *planet,
                            detail::metadata<planet_t>::ocean_coverage(),
                            "planet_details_",
                            true);
                    }
                    if (growth_uninhabitable < planet->growth_factor) {
                        add_panel_detail(
                            *planet,
                            detail::metadata<planet_t>::max_population(),
                            "planet_details_");
                        add_panel_detail(
                            *planet,
                            detail::metadata<planet_t>::growth_factor(),
                            "planet_details_",
                            true);
                        add_panel_detail(
                            *planet,
                            detail::metadata<
                                planet_t>::infrastructure_cost_factor(),
                            "planet_details_");
                    }
                }
                if (0 <= planet->water) {
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::water(),
                        "planet_details_");
                }
                if (0 <= planet->food) {
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::food(),
                        "planet_details_");
                }
                if (0 <= planet->energy) {
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::energy(),
                        "planet_details_");
                }
                if (0 <= planet->metal) {
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::metal(),
                        "planet_details_");
                }
                if (0 <= planet->fuel_minerals) {
                    add_panel_detail(
                        *planet,
                        detail::metadata<planet_t>::fuel_minerals(),
                        "planet_details_");
                }

                // TODO: effects

                // TODO: settlements
            }

            return FReply::Handled();
        };

        FButtonStyle & style = button_styles_[i];
        style.Normal = rt->brush();
        style.Hovered = rt->brush();
        style.Pressed = rt->brush();

        hbox_->AddSlot().AutoWidth().Padding(
            padding,
            0)[SNew(SBox)
                   .WidthOverride(object_render_size)
                   .HeightOverride(
                       object_render_size)[SNew(SButton)
                                               .ButtonStyle(&style)
                                               .OnClicked_Lambda(handle_click)
                                               .IsFocusable(false)]];
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
