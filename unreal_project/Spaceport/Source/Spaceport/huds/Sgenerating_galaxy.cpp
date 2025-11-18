#include "Sgenerating_galaxy.h"
#include "game_instance.h"
#include "widgets/Sstyled_progress_bar.h"
#include "widgets/Sstyled_text_block.h"

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Sgenerating_galaxy::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    TSharedPtr<SVerticalBox> vbox;

    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 127)))]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.3, 0.3, 0.7, 0.7))[
            SNew(SVerticalBox)
            +SVerticalBox::Slot().FillHeight(2)
            +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                SNew(Sstyled_text_block)
                .Text(loc_text(TEXT("generating_galaxy")))
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))
            ]
            +SVerticalBox::Slot().FillHeight(1)
            +SVerticalBox::Slot().MinHeight(80).VAlign(VAlign_Fill)[
                SAssignNew(bar_, Sstyled_progress_bar).Percent(0.0f)
            ]
            +SVerticalBox::Slot().FillHeight(2)
        ]
    ];
}

void Sgenerating_galaxy::percent_complete(int p)
{
    bar_->SetPercent(p / 100.0f);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
