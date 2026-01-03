#include "Ssystem_map_ui.h"
#include "Aplayer_controller.h"
#include "Arender_target.h"
#include "utility.hpp"
#include "game_data_formatters.hpp"
#include <ui_defaults.h>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
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

void Ssystem_map_ui::Construct(FArguments const & args)
{
    ChildSlot
        [SNew(SOverlay) +
         SOverlay::Slot()
             .HAlign(HAlign_Fill)
             .VAlign(VAlign_Bottom)
             .Padding(0, 10)[SAssignNew(hbox_, SHorizontalBox)]];
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

void Ssystem_map_ui::reset()
{
    system_id_ = -1;
    hbox_->ClearChildren();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
