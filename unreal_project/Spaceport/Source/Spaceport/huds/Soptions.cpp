#include "Soptions.h"
#include "game_instance.h"
#include "utility.hpp"
#include "Widgets/Skey_binding_button.h"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Stab_panel.h"

#include <SlateOptMacros.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    std::pair<FString, TSharedPtr<SWidget>> gameplay_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults)
    {
        auto panel = SNew(SOverlay); // TODO

        auto const apply = [=] {
            // TODO
        };
        auto const restore = [=] {
            // TODO
        };

        apply_changes = apply;
        restore_defaults = restore;

        return {TEXT("gameplay"), panel};
    }

    std::pair<FString, TSharedPtr<SWidget>> video_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults)
    {
        auto panel = SNew(SOverlay); // TODO

        auto const apply = [=] {
            // TODO
        };
        auto const restore = [=] {
            // TODO
        };

        apply_changes = apply;
        restore_defaults = restore;

        return {TEXT("video"), panel};
    }

    std::pair<FString, TSharedPtr<SWidget>> audio_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults)
    {
        auto panel = SNew(SOverlay); // TODO

        auto const apply = [=] {
            // TODO
        };
        auto const restore = [=] {
            // TODO
        };

        apply_changes = apply;
        restore_defaults = restore;

        return {TEXT("audio"), panel};
    }

    std::pair<FString, TSharedPtr<SWidget>> controls_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults)
    {
        std::shared_ptr remappings =
            std::make_shared<std::vector<std::function<void()>>>();

        auto const apply = [remappings] {
            for (auto & remapping : *remappings) {
                remapping();
            }
            remappings->clear();
            if (auto * pc = player_controller_base())
                pc->save_user_input_mappings();
        };
        auto const restore = [=] {
            // TODO
        };

        TSharedPtr<SVerticalBox> vbox;
        auto retval =
            SNew(SBox)
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                    [SNew(SScrollBox) +
                     SScrollBox::Slot()[SAssignNew(vbox, SVerticalBox)]];

        auto * pc = player_controller_base();
        check(pc);
        TArray<FEnhancedActionKeyMapping> curr_mappings =
            pc->player_mappable_action_key_mappings();

        // This extremely odd skipping code below is here to deal with this:
        // when I iterate without skipping, there is a mapping for each key,
        // along with all permutations of modifiers.  Since a 2d axis has four
        // buttons, and each one has a unique set of modifers to distinguish
        // the meaning of each of the four buttons in the axis, you get 16
        // mappings instead of four.  Sigh.

        // NB: console command for showing all key mappings:
        // `ShowDebug EnhancedInput`

        UInputAction const * prev_input_action = nullptr;
        int iterations_to_skip = 0;
        for (auto const & mapping : curr_mappings) {
            UInputAction const * input_action = mapping.Action;
            bool first = input_action != prev_input_action;
            if (first)
                iterations_to_skip = 0;

            if (iterations_to_skip) {
                --iterations_to_skip;
                continue;
            }

            bool y_axis = false;
            bool negative = false;
            for (auto const & mod : mapping.Modifiers) {
                if (Cast<UInputModifierSwizzleAxis>(mod.Get()))
                    y_axis = true;
                if (Cast<UInputModifierNegate>(mod.Get()))
                    negative = true;
            }
            bool const axis_2d =
                input_action->ValueType == EInputActionValueType::Axis2D;
            if (axis_2d && iterations_to_skip == 0)
                iterations_to_skip = 4;

            FName const name = mapping.GetMappingName();
            if (axis_2d && first) {
                // Add a row that indicates what all these Axis keys are
                // called, together as an axis.
                vbox->AddSlot().AutoHeight()
                    [SNew(SHorizontalBox) +
                     SHorizontalBox::Slot()[SNew(Sstyled_text_block)
                                                .Text(loc_text(
                                                    name.ToString()))] +
                     SHorizontalBox::Slot().FillWidth(1) +
                     SHorizontalBox::Slot().MinWidth(200)];
            }

            FString::ElementType const * axis_names[4] = {
                TEXT("right"), TEXT("up"), TEXT("left"), TEXT("down")};
            FKey key = mapping.Key;
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight()[SAssignNew(hbox, SHorizontalBox)];
            FText button_text;
            if (axis_2d) {
                button_text = FText::Format(
                    FTextFormat(FText::FromString(TEXT("      {0}"))),
                    loc_text(axis_names[(int)negative * 2 + (int)y_axis]));
            } else {
                button_text = FText::FromString(name.ToString());
            }
            hbox->AddSlot()[SNew(Sstyled_text_block)
                                .Text(button_text)];
            hbox->AddSlot().FillWidth(1);
            TSharedPtr<Skey_binding_button> button;
            hbox->AddSlot().MinWidth(
                200)[SAssignNew(button, Skey_binding_button)
                         .name(name)
                         .key(key.ToString())];
            button->rebind_action_target(*remappings);

            prev_input_action = input_action;
        }

        apply_changes = apply;
        restore_defaults = restore;

        return {TEXT("controls"), retval};
    }
}

void Soptions::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_font(ui_defaults().title_font_path_);

    TSharedPtr<Sstyled_button> apply_button;

    // clang-format off
    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0, 1, 0.1))
        .Offset(FMargin(25, 25, 25, 0))[
            SNew(SBox).HAlign(HAlign_Left).VAlign(VAlign_Top)[
                SNew(Sstyled_text_block)
                .Text(loc_text(TEXT("options")))
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))]
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0.1, 1, 0.9))
        .Offset(FMargin(25, 10, 25, 0))[
            SAssignNew(tab_panel_, Stab_panel)
            .selected_tab_color(FColor(218, 165, 32))
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0.9, 1, 1))
        .Offset(FMargin(25, 0, 25, 25))[
            SNew(SHorizontalBox)

            +SHorizontalBox::Slot().AutoWidth()[
                SAssignNew(apply_button, Sstyled_button)
                .Text(loc_text(TEXT("apply")))
                .OnClicked_Lambda([this] {
                    curr_panel_info().apply_changes_();
                    return FReply::Handled();
                })
            ]

            +SHorizontalBox::Slot().AutoWidth()[
                SNew(Sstyled_button)
                .Text(loc_text(TEXT("cancel")))
                .OnClicked_Lambda([this] {
                    if (auto * hud = hud_base())
                        hud->remove_widget(*this);
                    return FReply::Handled();
                })
            ]

            +SHorizontalBox::Slot().FillWidth(1)

            +SHorizontalBox::Slot().AutoWidth()[
                SNew(Sstyled_button)
                .Text(loc_text(TEXT("restore_defaults")))
                .OnClicked_Lambda([this] {
                    curr_panel_info().restore_defaults_();
                    return FReply::Handled();
                })
            ]
        ]
    ]];
    // clang-format on

    std::vector<std::pair<FString, TSharedPtr<SWidget>>> options_panels;

    panel_infos_.push_back({});
    options_panels.push_back(gameplay_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_));

    panel_infos_.push_back({});
    options_panels.push_back(video_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_));

    panel_infos_.push_back({});
    options_panels.push_back(audio_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_));

    panel_infos_.push_back({});
    options_panels.push_back(controls_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_));

    tab_panel_->panels(options_panels.begin(), options_panels.end());
}

bool Soptions::cancelable() { return true; }

Soptions::panel_info const & Soptions::curr_panel_info() const
{
    auto it = panel_infos_.begin();
    std::advance(it, tab_panel_->index());
    return *it;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
