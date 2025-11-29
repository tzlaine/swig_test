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
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    std::pair<FString, TSharedPtr<SWidget>> gameplay_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults,
        bool & can_apply,
        TSharedPtr<Sstyled_button> apply_button)
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
        std::function<void()> & restore_defaults,
        bool & can_apply,
        TSharedPtr<Sstyled_button> apply_button)
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
        std::function<void()> & restore_defaults,
        bool & can_apply,
        TSharedPtr<Sstyled_button> apply_button)
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

    struct controls_to_default_info
    {
        FName name_;
        FKey default_key_;
        TSharedPtr<Skey_binding_button> button_;
    };

    std::pair<FString, TSharedPtr<SWidget>> controls_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults,
        bool & can_apply,
        TSharedPtr<Sstyled_button> apply_button)
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

        auto * pc = player_controller_base();
        check(pc);
        TMap<FKey, FKey> current_to_default_keys =
            pc->current_to_default_keys();

        std::shared_ptr defaults_vec =
            std::make_shared<std::vector<controls_to_default_info>>();
        auto const restore = [defaults_vec, pc, &can_apply, apply_button] {
            for (auto & info : *defaults_vec) {
                pc->remap_key(info.name_, info.default_key_);
                info.button_->set_text(
                    FText::FromString(info.default_key_.ToString()));
                info.button_->indicate_conflict(false);
            }
            can_apply = true;
            apply_button->SetEnabled(can_apply);
        };

        std::shared_ptr buttons =
            std::make_shared<std::vector<TSharedPtr<Skey_binding_button>>>();
        auto const key_rebound =
            [buttons, &can_apply, apply_button]() {
                std::vector<FString> all_keys(buttons->size());
                std::ranges::transform(
                    *buttons, all_keys.begin(), [](auto const & e) {
                        return e->curr_key().ToString();
                    });
                std::ranges::sort(all_keys);
                std::vector<FString> dupes;
                auto first = all_keys.begin();
                auto const last = all_keys.end() - 1;
                for (; first != last; ++first) {
                    auto next = std::next(first);
                    if (*first == *next)
                        dupes.push_back(std::move(*next));
                }

                can_apply = true;
                for (auto const & button : *buttons) {
                    bool const dupe =
                        std::ranges::find(
                            dupes, button->curr_key().ToString()) !=
                        dupes.end();
                    if (dupe)
                        can_apply = false;
                    button->indicate_conflict(dupe);
                }
                apply_button->SetEnabled(can_apply);
            };

        TSharedPtr<SVerticalBox> vbox;
        auto retval =
            SNew(SBox)
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                    [SNew(SScrollBox) +
                     SScrollBox::Slot()[SAssignNew(vbox, SVerticalBox)]];

        TArray<FEnhancedActionKeyMapping> curr_mappings =
            pc->player_mappable_action_key_mappings();

        // NB: console command for visualizing when the key mappings are used:
        // `ShowDebug EnhancedInput`

        UInputAction const * prev_input_action = nullptr;
        for (auto const & mapping : curr_mappings) {
            UInputAction const * input_action = mapping.Action;
            bool const first = input_action != prev_input_action;

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

            FName const name = mapping.GetMappingName();
            if (axis_2d && first) {
                // Add a row that indicates what all these Axis keys are
                // called, together as an axis.
                vbox->AddSlot().AutoHeight()
                    [SNew(SHorizontalBox) +
                     SHorizontalBox::Slot()[SNew(Sstyled_text_block)
                                                .Text(loc_text(
                                                    input_action->GetName()))] +
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
                button_text = loc_text(name.ToString());
            }
            hbox->AddSlot()[SNew(Sstyled_text_block)
                                .Text(button_text)];
            hbox->AddSlot().FillWidth(1);
            TSharedPtr<Skey_binding_button> button;
            hbox->AddSlot().MinWidth(200)
                [SAssignNew(button, Skey_binding_button).name(name).key(key)];
            button->rebind_action_target(remappings);
            button->notifier(key_rebound);
            buttons->push_back(button);

            check(current_to_default_keys.Contains(key));
            defaults_vec->push_back(
                {name, current_to_default_keys[key], button});

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
                SAssignNew(apply_button_, Sstyled_button)
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
        panel_infos_.back().restore_defaults_,
        panel_infos_.back().can_apply_,
        apply_button_));

    panel_infos_.push_back({});
    options_panels.push_back(video_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_,
        panel_infos_.back().can_apply_,
        apply_button_));

    panel_infos_.push_back({});
    options_panels.push_back(audio_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_,
        panel_infos_.back().can_apply_,
        apply_button_));

    panel_infos_.push_back({});
    options_panels.push_back(controls_panel(
        panel_infos_.back().apply_changes_,
        panel_infos_.back().restore_defaults_,
        panel_infos_.back().can_apply_,
        apply_button_));

    tab_panel_->panels(options_panels.begin(), options_panels.end());
    tab_panel_->panel_change_callback([this](int i) {
        apply_button_->SetEnabled(get_panel_info(i).can_apply_);
    });
}

bool Soptions::cancelable() { return true; }

Soptions::panel_info const & Soptions::get_panel_info(int i) const
{
    auto it = panel_infos_.begin();
    std::advance(it, i);
    return *it;
}

Soptions::panel_info const & Soptions::curr_panel_info() const
{
    return get_panel_info(tab_panel_->index());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
