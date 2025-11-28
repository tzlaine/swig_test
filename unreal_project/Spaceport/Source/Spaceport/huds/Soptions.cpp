#include "Soptions.h"
#include "game_instance.h"
#include "utility.hpp"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Stab_panel.h"

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    std::pair<FString, TSharedPtr<SWidget>> gameplay()
    {
        // TODO
        return {TEXT("gameplay"), SNew(SOverlay)};
    }

    std::pair<FString, TSharedPtr<SWidget>> video()
    {
        // TODO
        return {TEXT("video"), SNew(SOverlay)};
    }

    std::pair<FString, TSharedPtr<SWidget>> audio()
    {
        // TODO
        return {TEXT("audio"), SNew(SOverlay)};
    }

    std::pair<FString, TSharedPtr<SWidget>> controls()
    {
        // TODO
        return {TEXT("controls"), SNew(SOverlay)};
    }
}

void Soptions::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_font(ui_defaults().title_font_path_);

    TSharedPtr<Stab_panel> tab_panel;

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
        .Anchors(FAnchors(0, 0.1, 1, 0))
        .Offset(FMargin(25, 0, 25, 25))[
            SAssignNew(tab_panel, Stab_panel)
        ]
    ]];
    // clang-format on

    std::vector<std::pair<FString, TSharedPtr<SWidget>>> options_panels;
    options_panels.push_back(gameplay());
    options_panels.push_back(video());
    options_panels.push_back(audio());
    options_panels.push_back(controls());
    tab_panel->insert(options_panels.begin(), options_panels.end());
}

bool Soptions::cancelable() { return true; }

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
