#include "Ssave_load_dlg.h"
#include "game_instance.h"
#include "utility.hpp"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Sstyled_button.h"

#include <format>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    TSharedRef<ITableRow> make_row(
        Ssave_load_dlg::item_data data,
        TSharedRef<STableViewBase> const & table)
    {
        return SNew(STableRow<TSharedPtr<FString>>, table)
            .Padding(2.0f)[SNew(Sstyled_text_block)
                           .Text(FText::FromString(*data))];
    }

    std::filesystem::path
    default_filename(std::string nation, std::chrono::utc_clock::time_point t)
    {
        return std::format("{}-{:%F}", nation, t);
    }
}

void Ssave_load_dlg::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    saving_ = args._saving;
    in_game_ = args._in_game;

    FText action_text =
        loc_text(saving_ ? TEXT("save_game") : TEXT("load_game"));

    auto do_action = [this] {
        if (saving_) {
            if (filename_is_in_list()) {
                playing_hud()->do_after_confirming(
                    [this] {
                        if (auto * pc = player_controller())
                            pc->server_save_game(filename_);
                        hide();
                    },
                    TEXT("confirm_save_over"),
                    TEXT("previous_save_will_be_lost"),
                    TEXT("save_over"));
            } else {
                if (auto * pc = player_controller())
                    pc->server_save_game(filename_);
                hide();
            }
        } else if (in_game_) {
            playing_hud()->do_after_confirming(
                [this] {
                    if (auto * pc = player_controller_base())
                        pc->server_load_game(filename_);
                },
                TEXT("confirm_load"),
                TEXT("unsaved_progress_lost"),
                TEXT("load_game"));
        } else {
            if (auto * pc = player_controller_base())
                pc->server_load_game(filename_);
        }
        return FReply::Handled();
    };

    // TODO: Allow selecting one or more saves, and deleting them.

    // clang-format off
    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 200)))]

        +SConstraintCanvas::Slot().Anchors(FAnchors(0.4, 0.2, 0.6, 0.8))[
            SNew(SBorder).Padding(50.0f)[ // TODO: Use a styled one.

                SNew(SVerticalBox)
                +SVerticalBox::Slot().FillHeight(5)

                +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                    SNew(Sstyled_text_block)
                    .Text(action_text)
                    .Font(FSlateFontInfo(title_font,
                                         ui_defaults().title_font_size_))]

                +SVerticalBox::Slot().FillHeight(5)

                +SVerticalBox::Slot().FillHeight(15).HAlign(HAlign_Fill)[
                    SAssignNew(edit_, SEditableTextBox) // TODO: Use a styled one.
                    .OnTextCommitted_Lambda(
                        [do_action, this](FText const & text, ETextCommit::Type type) {
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

                +SVerticalBox::Slot().FillHeight(70).HAlign(HAlign_Fill)[
                    SAssignNew(list_box_, SListView<item_data>) // TODO: Use a styled one.
                    .ItemHeight(24) // TODO
                    .ListItemsSource(&items_)
                    .OnGenerateRow_Lambda(&make_row)
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

                +SVerticalBox::Slot().FillHeight(5)

                +SVerticalBox::Slot().AutoHeight()[
                    SNew(SHorizontalBox)

                    +SHorizontalBox::Slot().FillWidth(1)

                    +SHorizontalBox::Slot().AutoWidth()[
                        SAssignNew(action_button_, Sstyled_button).Text(action_text)
                        .OnClicked_Lambda(do_action)]

                    +SHorizontalBox::Slot().FillWidth(0.5)

                    +SHorizontalBox::Slot().AutoWidth()[
                        SNew(Sstyled_button).Text(loc_text(TEXT("cancel")))
                        .OnClicked_Lambda([this] {
                            hide();
                            return FReply::Handled();
                        })]

                    +SHorizontalBox::Slot().FillWidth(1)]

                +SVerticalBox::Slot().FillHeight(2)]]];
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
