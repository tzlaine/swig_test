#include "Sconfirm_dlg.h"
#include "game_instance.h"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Sstyled_button.h"

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Sconfirm_dlg::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    result_ptr_ = args._result_ptr;
    check(result_ptr_);

    TSharedPtr<SHorizontalBox> buttons_hbox;

    FText message_as_text = args._message_as_text;
    if (message_as_text.IsEmpty())
        message_as_text = loc_text(args._message);

    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 200)))]

        +SConstraintCanvas::Slot().Anchors(FAnchors(0.3, 0.3, 0.7, 0.7))[
            SNew(SBorder).Padding(50.0f)[ // TODO: Use a styled one.

                SNew(SVerticalBox)
                +SVerticalBox::Slot().FillHeight(2)

                +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                    SNew(Sstyled_text_block)
                    .Text(loc_text(args._title))
                    .Font(FSlateFontInfo(title_font,
                                         ui_defaults().title_font_size_))]

                +SVerticalBox::Slot().FillHeight(1)

                // TODO: Use a styled multiline text block.
                +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                    SNew(Sstyled_text_block).AutoWrapText(true).Text(message_as_text)]

                +SVerticalBox::Slot().FillHeight(2)

                +SVerticalBox::Slot().AutoHeight()[
                    SAssignNew(buttons_hbox, SHorizontalBox)]

                +SVerticalBox::Slot().FillHeight(2)]]];

    buttons_hbox->AddSlot().FillWidth(10);

    if (args._yes_button == args._no_button) {
        buttons_hbox->AddSlot().AutoWidth()[
            SNew(Sstyled_button).Text(loc_text(args._yes_button))
            .OnClicked_Lambda([this] {
                *result_ptr_ = result::no;
                return FReply::Handled();
            })];

        cancelable_ = true;
    } else {
        buttons_hbox->AddSlot().AutoWidth()[
            SNew(Sstyled_button).Text(loc_text(args._yes_button))
            .OnClicked_Lambda([this] {
                *result_ptr_ = result::yes;
                return FReply::Handled();
            })];

        buttons_hbox->AddSlot().FillWidth(0.5);

        buttons_hbox->AddSlot().AutoWidth()[
            SNew(Sstyled_button).Text(loc_text(args._no_button))
            .OnClicked_Lambda([this] {
                *result_ptr_ = result::no;
                return FReply::Handled();
            })];
    }

    buttons_hbox->AddSlot().FillWidth(10);
}

bool Sconfirm_dlg::cancelable()
{
    return cancelable_;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
