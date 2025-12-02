#include "Sconfirm_dlg.h"
#include "game_instance.h"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_border.h"
#include "widgets/Sstyled_scroll_box.h"
#include <ui_defaults.h>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Sconfirm_dlg::Construct(FArguments const & args)
{
    UFont * title_font = ui_defaults().title_font_.Get();

    result_ptr_ = args._result_ptr;
    check(result_ptr_);

    TSharedPtr<SHorizontalBox> buttons_hbox;

    // TODO: Also check if the message would wrap.... That probably requires
    // adding logic in Tick().
    bool message_contains_newlines = false;
    FText message_as_text = args._message_as_text;
    if (message_as_text.IsEmpty()) {
        message_as_text = loc_text(args._message);
        message_contains_newlines = args._message.Contains(TEXT("\n"));
    } else {
        message_contains_newlines =
            message_as_text.ToString().Contains(TEXT("\n"));
    }

    TSharedPtr<SOverlay> message_overlay;

    FAnchors const anchors(0.25, 0.3, 0.75, 0.7);
    int vertical_spacing = 10;
    // clang-format off
    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(anchors)[
            SNew(Sstyled_border)
        ]

        +SConstraintCanvas::Slot().Anchors(anchors).Offset(20)[
            SNew(SVerticalBox)

            +SVerticalBox::Slot()
            .AutoHeight().HAlign(HAlign_Center)
            .Padding(0, 0, 0, vertical_spacing)[
                SNew(Sstyled_text_block)
                .Text(loc_text(args._title))
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))]

            +SVerticalBox::Slot()
            .HAlign(HAlign_Fill)
            .Padding(0, 0, 0, vertical_spacing)[
                SAssignNew(message_overlay, SOverlay)
            ]

            +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Fill)[
                SAssignNew(buttons_hbox, SHorizontalBox)]
        ]
    ]];
    // clang-format on

    if (message_contains_newlines) {
        message_overlay->AddSlot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
                [STYLED_SCROLL_BOX()

                 + SScrollBox::Slot()[SNew(Sstyled_text_block)
                                          .Justification(ETextJustify::Left)
                                          .AutoWrapText(true)
                                          .Text(message_as_text)]];
    } else {
        message_overlay->AddSlot()
            .HAlign(HAlign_Fill)
            .VAlign(
                VAlign_Fill)[SNew(SBox)
                                 .HAlign(HAlign_Center)
                                 .VAlign(VAlign_Center)
                                     [SNew(Sstyled_text_block)
                                          .Justification(ETextJustify::Center)
                                          .AutoWrapText(true)
                                          .Text(message_as_text)]];
    }

    if (args._yes_button == args._no_button) {
        buttons_hbox->AddSlot()
            .HAlign(HAlign_Fill)
            .Padding(10, 0, 10, 0)[SNew(Sstyled_button)
                                       .Text(loc_text(args._yes_button))
                                       .OnClicked_Lambda([this] {
                                           *result_ptr_ = result::no;
                                           return FReply::Handled();
                                       })];

        cancelable_ = true;
    } else {
        buttons_hbox->AddSlot().FillWidth(50).Padding(
            10, 0, 10, 0)[SNew(Sstyled_button)
                              .Text(loc_text(args._yes_button))
                              .OnClicked_Lambda([this] {
                                  *result_ptr_ = result::yes;
                                  return FReply::Handled();
                              })];

        buttons_hbox->AddSlot().FillWidth(50).Padding(
            10, 0, 10, 0)[SNew(Sstyled_button)
                              .Text(loc_text(args._no_button))
                              .OnClicked_Lambda([this] {
                                  *result_ptr_ = result::no;
                                  return FReply::Handled();
                              })];
    }
}

bool Sconfirm_dlg::cancelable()
{
    return cancelable_;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
