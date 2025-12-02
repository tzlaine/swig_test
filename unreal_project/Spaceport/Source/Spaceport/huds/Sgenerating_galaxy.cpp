#include "Sgenerating_galaxy.h"
#include "game_instance.h"
#include "widgets/Sstyled_progress_bar.h"
#include "widgets/Sstyled_text_block.h"
#include <ui_defaults.h>

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

void Sgenerating_galaxy::Construct(FArguments const & args)
{
    UFont * title_font = ui_defaults().title_font_.Get();

    TSharedPtr<SVerticalBox> vbox;

    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.3, 0.3, 0.7, 0.7))[
            SNew(SVerticalBox)
            +SVerticalBox::Slot().FillHeight(50)
            +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                SNew(Sstyled_text_block)
                .Text(loc_text(TEXT("generating_galaxy")))
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))
            ]
            +SVerticalBox::Slot().MinHeight(20).Padding(0, 40)[
                SAssignNew(bar_, Sstyled_progress_bar).Percent(0.0f)
            ]
            +SVerticalBox::Slot().FillHeight(50)
        ]
    ]];
}

void Sgenerating_galaxy::percent_complete(int p)
{
    bar_->SetPercent(p / 100.0f);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
