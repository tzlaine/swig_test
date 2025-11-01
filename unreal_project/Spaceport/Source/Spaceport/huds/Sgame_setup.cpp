#include "Sgame_setup.h"
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
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/Text/STextBlock.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Sgame_setup::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 127)))]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.05, 0, 0.95, 0.125))[
            SNew(SVerticalBox)
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.05, 0.125, 0.95, 0.875))[
            SNew(SVerticalBox)
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.05, 0.875, 0.95, 1))[
            SNew(SVerticalBox)

            +SVerticalBox::Slot().FillHeight(1)

            +SVerticalBox::Slot().AutoHeight()[
                SNew(SHorizontalBox)

                +SHorizontalBox::Slot().FillWidth(10)

                +SHorizontalBox::Slot().FillWidth(2)[
                    SNew(Sstyled_button).Text(loc_text(TEXT("back")))
                    .OnClicked_Lambda([] {
                        UE_LOG(LogTemp, Warning, TEXT("Go back!"));
                        // TODO
                        return FReply::Handled();
                    })
                ]

                +SHorizontalBox::Slot().FillWidth(0.5)

                +SHorizontalBox::Slot().FillWidth(2)[
                    SNew(Sstyled_button).Text(loc_text(TEXT("play")))
                    .OnClicked_Lambda([] {
                        UE_LOG(LogTemp, Warning, TEXT("Start game"));
                        // TODO
                        return FReply::Handled();
                    })
                ]
            ]

            +SVerticalBox::Slot().FillHeight(1)
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
