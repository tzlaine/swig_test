#include "Soptions.h"
#include "audio_assets.h"
#include "game_instance.h"
#include "game_user_settings.h"
#include "utility.hpp"
#include "Widgets/Skey_binding_button.h"
#include "Widgets/Spip_rotator_button.h"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_check_box.h"
#include "widgets/Sstyled_slider.h"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Stab_panel.h"
#include <ui_defaults.h>

#include <utility>

#include <RHI.h>
#include <SlateOptMacros.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <GameFramework/GameUserSettings.h>
#include <Internationalization/Internationalization.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SBox.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/Layout/SScrollBox.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

bool operator==(FScreenResolutionRHI const & l, FScreenResolutionRHI const & r)
{
    return l.Width == r.Width && l.Height == r.Height;
}
bool operator<(FScreenResolutionRHI const & l, FScreenResolutionRHI const & r)
{
    if (l.Width < r.Width)
        return true;
    if (r.Width < l.Width)
        return false;
    return l.Height < r.Height;
}

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

    std::pair<FScreenResolutionArray, int>
    supported_screen_resolutions(FScreenResolutionRHI curr_resolution)
    {
        FScreenResolutionArray retval;
        RHIGetAvailableResolutions(retval, true);
        std::sort(begin(retval), end(retval));
        int const new_size =
            std::unique(begin(retval), end(retval)) - begin(retval);
        retval.SetNum(new_size);

        // Ensure that the current resolution is actually in the list.
        auto curr_resolution_it =
            std::lower_bound(begin(retval), end(retval), curr_resolution);
        int const pos = curr_resolution_it - begin(retval);
        if (curr_resolution_it == end(retval)) {
            retval.Add(curr_resolution);
        } else if (*curr_resolution_it != curr_resolution) {
            retval.SetNum(retval.Num() + 1);
            curr_resolution_it = begin(retval) + pos;
            std::copy_backward(
                curr_resolution_it, end(retval) - 1, end(retval));
            *curr_resolution_it = curr_resolution;
        }

        return {std::move(retval), pos};
    }

    FScreenResolutionRHI default_resolution(
        Ugame_user_settings & game_user_settings,
        FScreenResolutionArray const & all_resolutions)
    {
        game_user_settings.LoadSettings();
        game_user_settings.ValidateSettings();

        FIntPoint const settings_default =
            game_user_settings.GetDefaultResolution();
        if (settings_default.X && settings_default.Y) {
            return FScreenResolutionRHI{
                (uint32_t)settings_default.X, (uint32_t)settings_default.Y};
        }

        FIntPoint const desktop_resolution =
            game_user_settings.GetDesktopResolution();
        if (desktop_resolution.X && desktop_resolution.Y) {
            return FScreenResolutionRHI{
                (uint32_t)desktop_resolution.X, (uint32_t)desktop_resolution.Y};
        }

        if (!all_resolutions.IsEmpty())
            return all_resolutions.Last();

        return FScreenResolutionRHI{1280u, 720u};
    }

    struct video_defaults_and_buttons
    {
        TSharedPtr<Spip_rotator_button> resolution_button_;
        FScreenResolutionRHI resolution_;
        int resolution_index_;
        TSharedPtr<Spip_rotator_button> window_mode_button_;
        EWindowMode::Type window_mode_;
        int window_mode_index_;
        TSharedPtr<Sstyled_check_box> vsync_button_;
        bool vsync_;
        TSharedPtr<Spip_rotator_button> texture_quality_button_;
        int texture_quality_;
        TSharedPtr<Spip_rotator_button> aa_method_button_;
        int aa_method_;
        TSharedPtr<Spip_rotator_button> aa_quality_button_;
        int aa_quality_;
        TSharedPtr<Spip_rotator_button> planet_detail_button_;
        int planet_detail_;
    };

    std::pair<FString, TSharedPtr<SWidget>> video_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults,
        bool & can_apply,
        TSharedPtr<Sstyled_button> apply_button)
    {
        Ugame_user_settings * game_user_settings =
            Ugame_user_settings::get();

        std::shared_ptr apply_ops =
            std::make_shared<std::vector<std::function<void()>>>();
        auto const apply = [apply_ops, game_user_settings] {
            for (auto const & op : *apply_ops) {
                op();
            }
            game_user_settings->ApplySettings(false);
            game_user_settings->SaveSettings();
            apply_ops->clear();
        };

        std::shared_ptr defaults =
            std::make_shared<video_defaults_and_buttons>();
        auto const restore = [defaults, apply_ops] {
            apply_ops->clear();

            Ugame_user_settings * game_user_settings =
                Ugame_user_settings::get();
            defaults->resolution_button_->select(defaults->resolution_index_);
            defaults->window_mode_button_->select(defaults->window_mode_index_);
            defaults->vsync_button_->SetIsChecked(
                defaults->vsync_ ? ECheckBoxState::Checked
                                 : ECheckBoxState::Unchecked);
            apply_ops->push_back([enable = defaults->vsync_, defaults] {
                Ugame_user_settings * game_user_settings =
                    Ugame_user_settings::get();
                game_user_settings->SetVSyncEnabled(enable);
                defaults->window_mode_button_->select(
                    defaults->window_mode_index_);
            });
            defaults->texture_quality_button_->select(
                defaults->texture_quality_);
            defaults->aa_method_button_->select(defaults->aa_method_);
            defaults->aa_quality_button_->select(defaults->aa_quality_);
            defaults->planet_detail_button_->select(defaults->planet_detail_);
        };

        TSharedPtr<SVerticalBox> vbox;
        auto retval =
            SNew(SBox)
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                    [SNew(SScrollBox) +
                     SScrollBox::Slot()[SAssignNew(vbox, SVerticalBox)]];

        UFont * title_font = ui_defaults().title_font_.Get();
        vbox->AddSlot()
            .MinHeight(50)
            .VAlign(VAlign_Center)
            .Padding(
                0,
                40,
                0,
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("display_settings")))
                        .Font(FSlateFontInfo(
                            title_font, ui_defaults().font_size_ * 4 / 3))];

        FScreenResolutionRHI curr_resolution = {0};
        {
            FIntPoint const res = game_user_settings->GetScreenResolution();
            curr_resolution.Width = res.X;
            curr_resolution.Height = res.Y;
        }
        auto [all_resolutions, curr_resolution_pos] =
            supported_screen_resolutions(curr_resolution);
        defaults->resolution_ =
            default_resolution(*game_user_settings, all_resolutions);
        {
            auto curr_resolution_it = std::lower_bound(
                begin(all_resolutions),
                end(all_resolutions),
                defaults->resolution_);
            defaults->resolution_index_ =
                curr_resolution_it - begin(all_resolutions);
        }
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("screen_resolution")))];
            TArray<FText> settings;
            for (auto const & res : all_resolutions) {
                settings.Add(FText::Format(
                    FText::FromString("{0} x {1}"), res.Width, res.Height));
            }
            hbox->AddSlot().FillWidth(20);
            TSharedPtr<Spip_rotator_button> button;
            hbox->AddSlot().FillWidth(
                40)[SAssignNew(button, Spip_rotator_button).settings(settings)];
            button->select(curr_resolution_pos);
            button->notifier(
                [resolutions = std::move(all_resolutions), apply_ops](int i) {
                    auto const resolution = resolutions[i];
                    apply_ops->push_back([resolution] {
                        Ugame_user_settings * game_user_settings =
                            Ugame_user_settings::get();
                        game_user_settings->SetScreenResolution(
                            FIntPoint(resolution.Width, resolution.Height));
                    });
                });
            defaults->resolution_button_ = button;
        }

        EWindowMode::Type const window_mode =
            game_user_settings->GetFullscreenMode();
        EWindowMode::Type const default_window_mode =
            game_user_settings->GetDefaultWindowMode();
        defaults->window_mode_ = default_window_mode;
        defaults->window_mode_index_ = (int)default_window_mode;
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("window_mode")))];
            hbox->AddSlot().FillWidth(20);
            TArray<FText> settings;
            settings.Add(loc_text(TEXT("fullscreen")));
            settings.Add(loc_text(TEXT("windowed_fullscreen")));
            settings.Add(loc_text(TEXT("windowed")));
            TSharedPtr<Spip_rotator_button> button;
            hbox->AddSlot().FillWidth(
                40)[SAssignNew(button, Spip_rotator_button).settings(settings)];
            button->select((int)window_mode);
            button->notifier([apply_ops](int i) {
                apply_ops->push_back([window_mode = (EWindowMode::Type)i] {
                    Ugame_user_settings * game_user_settings =
                        Ugame_user_settings::get();
                    game_user_settings->SetFullscreenMode(window_mode);
                });
            });
            defaults->window_mode_button_ = button;
        }

        bool const vsync = game_user_settings->IsVSyncEnabled();
        defaults->vsync_ = false;
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("enable_vsync")))];
            hbox->AddSlot().FillWidth(20);
            TSharedPtr<Sstyled_check_box> button;
            hbox->AddSlot().FillWidth(40).HAlign(HAlign_Center)
                [SAssignNew(button, Sstyled_check_box)
                     .IsChecked(
                         vsync ? ECheckBoxState::Checked
                               : ECheckBoxState::Unchecked)
                     .OnCheckStateChanged_Lambda([apply_ops](
                                                     ECheckBoxState state) {
                         apply_ops->push_back(
                             [enable = state == ECheckBoxState::Checked] {
                                 Ugame_user_settings * game_user_settings =
                                     Ugame_user_settings::get();
                                 game_user_settings->SetVSyncEnabled(enable);
                             });
                     })];
            defaults->vsync_button_ = button;
        }

        vbox->AddSlot()
            .MinHeight(50)
            .VAlign(VAlign_Center)
            .Padding(
                0,
                40,
                0,
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("graphics_settings")))
                        .Font(FSlateFontInfo(
                            title_font, ui_defaults().font_size_ * 4 / 3))];

        TArray<FText> graphics_settings_0_4;
        graphics_settings_0_4.Add(loc_text(TEXT("graphics_low")));
        graphics_settings_0_4.Add(loc_text(TEXT("graphics_medium")));
        graphics_settings_0_4.Add(loc_text(TEXT("graphics_high")));
        graphics_settings_0_4.Add(loc_text(TEXT("graphics_epic")));
        graphics_settings_0_4.Add(loc_text(TEXT("graphics_cinematic")));

        int const texture_quality = game_user_settings->texture_quality;
        defaults->texture_quality_ = 3;
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("texture_quality")))];
            hbox->AddSlot().FillWidth(20);
            TSharedPtr<Spip_rotator_button> button;
            hbox->AddSlot().FillWidth(
                40)[SAssignNew(button, Spip_rotator_button)
                        .settings(graphics_settings_0_4)];
            button->select(texture_quality);
            button->notifier([apply_ops](int i) {
                apply_ops->push_back([i] {
                    Ugame_user_settings * game_user_settings =
                        Ugame_user_settings::get();
                    game_user_settings->texture_quality = i;
                });
            });
            defaults->texture_quality_button_ = button;
        }

        int const default_aa_method = AAM_TSR;
        int const aa_method = game_user_settings->aa_method;
        defaults->aa_method_ = default_aa_method;
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("aa_method")))];
            hbox->AddSlot().FillWidth(20);
            TArray<FText> settings;
            settings.Add(loc_text(TEXT("aa_off")));
            settings.Add(loc_text(TEXT("aa_fxaa")));
            settings.Add(loc_text(TEXT("aa_taa")));
            settings.Add(loc_text(TEXT("aa_msaa")));
            settings.Add(loc_text(TEXT("aa_tsr")));
            TSharedPtr<Spip_rotator_button> button;
            hbox->AddSlot().FillWidth(
                40)[SAssignNew(button, Spip_rotator_button).settings(settings)];
            button->select(aa_method);
            button->notifier([apply_ops](int i) {
                apply_ops->push_back([i] {
                    Ugame_user_settings * game_user_settings =
                        Ugame_user_settings::get();
                    game_user_settings->aa_method = i;
                });
            });
            defaults->aa_method_button_ = button;
        }

        int const aa_quality = game_user_settings->aa_quality;
        defaults->aa_quality_ = 3;
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("aa_quality")))];
            hbox->AddSlot().FillWidth(20);
            TSharedPtr<Spip_rotator_button> button;
            hbox->AddSlot().FillWidth(
                40)[SAssignNew(button, Spip_rotator_button)
                        .settings(graphics_settings_0_4)];
            button->select(aa_quality);
            button->notifier([apply_ops](int i) {
                apply_ops->push_back([i] {
                    Ugame_user_settings * game_user_settings =
                        Ugame_user_settings::get();
                    game_user_settings->aa_quality = i;
                });
            });
            defaults->aa_quality_button_ = button;
        }

        int const planet_detail = game_user_settings->planet_detail;
        defaults->planet_detail_ = 3;
        {
            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block)
                        .Text(loc_text(TEXT("planet_detail")))];
            hbox->AddSlot().FillWidth(20);
            TSharedPtr<Spip_rotator_button> button;
            hbox->AddSlot().FillWidth(
                40)[SAssignNew(button, Spip_rotator_button)
                        .settings(graphics_settings_0_4)];
            button->select(planet_detail);
            button->notifier([apply_ops](int i) {
                apply_ops->push_back([i] {
                    Ugame_user_settings * game_user_settings =
                        Ugame_user_settings::get();
                    game_user_settings->planet_detail = i;
                });
            });
            defaults->planet_detail_button_ = button;
        }

        // TODO: star detail?

        apply_changes = apply;
        restore_defaults = restore;

        return {TEXT("video"), retval};
    }

    const float default_volume = 1.0f;

    float volume_of(FString const & mix, FString const & reference_mix)
    {
        if (mix == reference_mix)
            return Ugame_user_settings::get()->volume_levels[mix];
        return Ugame_user_settings::get()->volume_levels[mix] /
               Ugame_user_settings::get()->volume_levels[reference_mix];
    }

    std::pair<FString, TSharedPtr<SWidget>> audio_panel(
        std::function<void()> & apply_changes,
        std::function<void()> & restore_defaults,
        bool & can_apply,
        TSharedPtr<Sstyled_button> apply_button)
    {
        Ugame_user_settings * game_user_settings =
            Ugame_user_settings::get();

        // TODO: Audio is a little different.  Instead of entirely deferring
        // the changes until the user hits Apply, we should probably apply
        // them immediately, and then roll them back when hiding the options
        // screen.

        std::shared_ptr apply_ops =
            std::make_shared<std::vector<std::function<void()>>>();
        auto const apply = [apply_ops, game_user_settings] {
            for (auto const & op : *apply_ops) {
                op();
            }
            apply_ops->clear();
            game_user_settings->ApplySettings(false);
            game_user_settings->SaveSettings();
        };

        std::shared_ptr sliders_vec =
            std::make_shared<std::vector<TSharedPtr<Sstyled_slider>>>();
        auto const restore = [sliders_vec, apply_ops] {
            apply_ops->clear();
            for (auto const & slider : *sliders_vec) {
                slider->SetValue(default_volume);
                slider->value_changed(default_volume);
            }
        };

        TSharedPtr<SVerticalBox> vbox;
        auto retval =
            SNew(SBox)
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                    [SNew(SScrollBox) +
                     SScrollBox::Slot()[SAssignNew(vbox, SVerticalBox)]];

        auto const make_number_text = [](auto value) {
            FNumberFormattingOptions number_text_options;
            number_text_options.MaximumFractionalDigits = 0;
            return FText::AsNumber(value * 100, &number_text_options);
        };

        FString const master_name = TEXT("master_sound");

        for (auto const & [name, mix] : audio_assets().sound_mixes_) {
            FString const category_label = name + TEXT("_volume");

            bool const master = category_label.Contains(TEXT("master"));
            double const curr_value = volume_of(name, master_name);

            TSharedPtr<SHorizontalBox> hbox;
            vbox->AddSlot().AutoHeight().Padding(
                0, 0, 0, 10)[SAssignNew(hbox, SHorizontalBox)];

            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block).Text(loc_text(category_label))];
            hbox->AddSlot().FillWidth(20);
            TSharedPtr<Sstyled_slider> slider;
            TSharedPtr<Sstyled_text_block> text;
            hbox->AddSlot().FillWidth(40).VAlign(VAlign_Fill)
                [SNew(SOverlay) +
                 SOverlay::Slot()
                     .HAlign(HAlign_Center)
                     .ZOrder(1)[SAssignNew(text, Sstyled_text_block)
                                    .Text(make_number_text(curr_value))] +
                 SOverlay::Slot().ZOrder(0).VAlign(VAlign_Center)
                     [SAssignNew(slider, Sstyled_slider)
                          .OnValueChanged_Lambda([=](auto value) {
                              text->SetText(make_number_text(value));
                              if (master) {
                                  apply_ops->push_back([=] {
                                      float const prev_value =
                                          volume_of(master_name, master_name);
                                      for (auto const & [name, mix] :
                                           audio_assets().sound_mixes_) {
                                          float this_value = value;
                                          if (name != master_name) {
                                              this_value =
                                                  volume_of(name, name) /
                                                  prev_value * value;
                                          }
                                          Ugame_user_settings::get()
                                              ->volume_levels[name] =
                                              this_value;
                                      }
                                  });
                              } else {
                                  apply_ops->push_back([=] {
                                      float const master_volume =
                                          volume_of(master_name, master_name);
                                      Ugame_user_settings::get()
                                          ->volume_levels[name] =
                                          value * master_volume;
                                  });
                              }
                          })
                          .Value(curr_value)]];
            sliders_vec->push_back(slider);
        }

        apply_changes = apply;
        restore_defaults = restore;

        return {TEXT("audio"), retval};
    }

    struct controls_to_default_info
    {
        FName name_;
        FKey default_key_;
        TSharedPtr<Skey_binding_button> button_;
    };

    // TODO: We don't gracefully handle mouse events; we only detect key
    // presses.  If you change off of the camera zoom settings here, you can't
    // get them back (because they use the mouse wheel).
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
            if (auto * pc = player_controller())
                pc->save_user_input_mappings();
        };

        auto * pc = player_controller();
        check(pc);
        TMap<FKey, FKey> current_to_default_keys =
            pc->current_to_default_keys();

        std::shared_ptr defaults_vec =
            std::make_shared<std::vector<controls_to_default_info>>();
        auto const restore =
            [defaults_vec, pc, &can_apply, apply_button, remappings] {
                for (auto & info : *defaults_vec) {
                    pc->remap_key(info.name_, info.default_key_);
                    info.button_->set_text(
                        FText::FromString(info.default_key_.ToString()));
                    info.button_->indicate_conflict(false);
                }
                can_apply = true;
                apply_button->SetEnabled(can_apply);
                remappings->clear();
            };

        std::shared_ptr buttons =
            std::make_shared<std::vector<TSharedPtr<Skey_binding_button>>>();
        auto const key_rebound = [buttons, &can_apply, apply_button]() {
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
                    std::ranges::find(dupes, button->curr_key().ToString()) !=
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
                     SHorizontalBox::Slot().FillWidth(
                         40)[SNew(Sstyled_text_block)
                                 .Text(loc_text(input_action->GetName()))] +
                     SHorizontalBox::Slot().FillWidth(40) +
                     SHorizontalBox::Slot().FillWidth(20)];
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
            hbox->AddSlot().FillWidth(
                40)[SNew(Sstyled_text_block).Text(button_text)];
            hbox->AddSlot().FillWidth(40);
            TSharedPtr<Skey_binding_button> button;
            hbox->AddSlot().FillWidth(20)
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
    UFont * title_font = ui_defaults().title_font_.Get();

    // clang-format off
    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0, 1, 0.1))
        .Offset(FMargin(20, 20, 20, 0))[
            SNew(SBox).HAlign(HAlign_Left).VAlign(VAlign_Top)[
                SNew(Sstyled_text_block)
                .Text(loc_text(TEXT("options")))
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))]
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0.1, 1, 0.9))
        .Offset(FMargin(20, 10, 20, 0))[
            SAssignNew(tab_panel_, Stab_panel)
            .selected_tab_color(FColor(218, 165, 32))
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0.9, 1, 1))
        .Offset(FMargin(20, 0, 20, 20))[
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
