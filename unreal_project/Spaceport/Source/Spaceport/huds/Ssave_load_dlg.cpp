#include "Ssave_load_dlg.h"
#include "game_instance.h"
#include "utility.hpp"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Sstyled_border.h"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_editable_text_box.h"
#include "audio_assets.h"
#include <ui_defaults.h>

#include <format>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    std::filesystem::path
    default_filename(std::string nation, std::chrono::utc_clock::time_point t)
    {
        return std::format("{}-{:%F}", nation, t);
    }
}

void Ssave_load_dlg::Construct(FArguments const & args)
{
    UFont * title_font = ui_defaults().title_font_.Get();

    saving_ = args._saving;
    in_game_ = args._in_game;

    FText action_text =
        loc_text(saving_ ? TEXT("save_game") : TEXT("load_game"));

    auto do_action = [this] {
        if (saving_) {
            if (filename_is_in_list()) {
                hud()->do_after_confirming(
                    [this] {
                        if (auto * pc = player_controller())
                            pc->server_save_game(filename_);
                        if (auto * hud = hud())
                            hud->remove_widget(*this);
                    },
                    TEXT("confirm_save_over"),
                    TEXT("previous_save_will_be_lost"),
                    TEXT("save_over"));
            } else {
                if (auto * pc = player_controller())
                    pc->server_save_game(filename_);
                if (auto * hud = ::hud())
                    hud->remove_widget(*this);
            }
        } else if (in_game_) {
            hud()->do_after_confirming(
                [this] {
                    // The button click sounds that should paly normally gets
                    // cut off by the level load.  We have to explicitly play it
                    // here.
                    Ugame_instance::get()->play_sound_across_level_loads(
                        audio_assets().click_cue_);
                    if (auto * pc = player_controller())
                        pc->server_load_game(filename_);
                },
                TEXT("confirm_load"),
                TEXT("unsaved_progress_lost"),
                TEXT("load_game"));
        } else {
            // The button click sounds that should paly normally gets cut
            // off by the level load.  We have to explicitly play it here.
            Ugame_instance::get()->play_sound_across_level_loads(
                audio_assets().click_cue_);
            if (auto * pc = player_controller())
                pc->server_load_game(filename_);
        }
        return FReply::Handled();
    };

    // TODO: Allow selecting one or more saves, and deleting them.

    FAnchors const anchors(0.35, 0.15, 0.65, 0.85);
    int vertical_spacing = 15;
    // clang-format off
    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(anchors)[
            SNew(Sstyled_border)
        ]

        +SConstraintCanvas::Slot().Anchors(anchors).Offset(20)[
            SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                SNew(Sstyled_text_block)
                .Text(action_text)
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))]

            +SVerticalBox::Slot()
            .AutoHeight().HAlign(HAlign_Fill)
            .Padding(0, vertical_spacing, 0, vertical_spacing)[
                SAssignNew(edit_, Sstyled_editable_text_box)
                .OnTextCommitted_Lambda([do_action, this](
                    FText const & text, ETextCommit::Type type) {
                        if (type != ETextCommit::OnEnter)
                            return;
                        filename_ = text.ToString();
                        do_action();
                    })
                .OnTextChanged_Lambda([this](FText const & text) {
                    filename_ = text.ToString();
                    action_button_->SetEnabled(filename_is_well_formed());
                })
            ]

            +SVerticalBox::Slot().FillHeight(100).HAlign(HAlign_Fill)[
                SAssignNew(list_box_, Sstyled_list_view<item_data>)
#pragma warning(push)
#pragma warning(disable : 4996)
                .ItemHeight(ui_defaults().font_size_)
#pragma warning(pop)
                .ListItemsSource(&items_)
                .OnGenerateRow_Lambda(
                    [](TSharedPtr<FString> text,
                       TSharedRef<STableViewBase> const & table) {
                        return ui_defaults().make_row(text, table);
                    })
                .SelectionMode(ESelectionMode::Single)
                .OnMouseButtonDoubleClick_Lambda(
                    [do_action, this](Ssave_load_dlg::item_data data) {
                        if (!data)
                            return;
                        filename_ = *data;
                        do_action();
                    })
                .OnSelectionChanged_Lambda(
                    [this](Ssave_load_dlg::item_data data,
                           ESelectInfo::Type) {
                        if (data)
                            filename_ = *data;
                        else
                            filename_.Reset();
                        edit_->SetText(FText::FromString(filename_));
                        action_button_->SetEnabled(filename_is_well_formed());
                    })]

            +SVerticalBox::Slot()
            .AutoHeight().HAlign(HAlign_Fill)
            .Padding(0, vertical_spacing, 0, 0)[
                SNew(SHorizontalBox)

                +SHorizontalBox::Slot().FillWidth(50).Padding(10, 0, 10, 0)[
                    SAssignNew(action_button_, Sstyled_button).Text(action_text)
                    .OnClicked_Lambda(do_action)]

                +SHorizontalBox::Slot().FillWidth(50).Padding(10, 0, 10, 0)[
                    SNew(Sstyled_button).Text(loc_text(TEXT("cancel")))
                    .OnClicked_Lambda([this] {
                        if (auto * hud = ::hud())
                            hud->remove_widget(*this);
                        return FReply::Handled();
                    })]
            ]
        ]
    ]];
    // clang-format on

    if (saving_) {
        FString s = to_fstring(default_filename("TODO", std::chrono::utc_clock::now())) ;
        edit_->SetText(FText::FromString(s));
    }

    action_button_->SetEnabled(filename_is_well_formed());
}

void Ssave_load_dlg::saves_changed(TArray<FString> const & files)
{
    items_.SetNum(files.Num());
    std::transform(begin(files), end(files), begin(items_), [](auto const & e) {
        return item_data(new FString(e));
    });
    if (list_box_)
        list_box_->RequestListRefresh();
}

bool Ssave_load_dlg::cancelable() { return true; }

bool Ssave_load_dlg::filename_is_well_formed() const
{
    if (saving_)
        return !filename_.IsEmpty();
    return filename_is_in_list();
}

bool Ssave_load_dlg::filename_is_in_list() const
{
    return std::any_of(begin(items_), end(items_), [this](auto const & e) {
        return e && *e == filename_;
    });
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
