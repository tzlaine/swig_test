#include "Ssystem_map_ui.h"
#include "Aplayer_controller.h"
#include "Arender_target.h"
#include "utility.hpp"
#include <ui_defaults.h>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SBoxPanel.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

int const object_render_size = 150; // TODO

void Ssystem_map_ui::Construct(FArguments const & args)
{
    ChildSlot
        [SNew(SOverlay) +
         SOverlay::Slot()
             .HAlign(HAlign_Fill)
             .VAlign(VAlign_Fill)
                 [SNew(SVerticalBox) + SVerticalBox::Slot().FillHeight(1) +
                  SVerticalBox::Slot()
                      .MinHeight(object_render_size)
                      .MaxHeight(object_render_size)[SAssignNew(
                          hbox_, SHorizontalBox)]]];
}

void Ssystem_map_ui::rebuild()
{
    reset();

    auto * pc = player_controller();
    std::span<Arender_target const * const> object_renders =
        pc->system_map_object_renders();

    for (auto * rt : object_renders) {
        hbox_->AddSlot().MinWidth(
            object_render_size)[SNew(SImage).Image(&rt->brush())];
    }

    hbox_->AddSlot().FillWidth(1);
}

void Ssystem_map_ui::reset() { hbox_->ClearChildren(); }

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
