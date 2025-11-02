#include "Sgame_setup.h"
#include "Aplayer_controller.h"
#include "game_instance.h"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_spin_box.h"
#include "widgets/Sstyled_text_block.h"

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
            +SVerticalBox::Slot().FillHeight(1)
            +SVerticalBox::Slot().AutoHeight()[
                SNew(Sstyled_text_block)
                .Text(loc_text(TEXT("game_setup_title")))
                .Font(FSlateFontInfo(title_font, 48))
            ]
            +SVerticalBox::Slot().FillHeight(1)
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.05, 0.125, 0.95, 0.875))[
            SAssignNew(vbox_, SVerticalBox)
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
                        UE_LOG(LogTemp, Warning, TEXT("ENTER Back CLICK")); // TODO
                        Aplayer_controller * pc = Cast<Aplayer_controller>(
                            ::world()->GetFirstPlayerController());
                        pc->server_quit_to_menu();
                        UE_LOG(LogTemp, Warning, TEXT("EXIT Back CLICK")); // TODO
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

    vbox_->AddSlot().MinSize(50).AutoHeight()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_text_block)
            .Text(loc_text(TEXT("setup_total_systems")))]
        +SHorizontalBox::Slot().FillWidth(1)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_int_spin_box)
            .MinValue(1)
            .Value_Lambda([this]{ return params_.systems_per_hex;})
            .MaxValue(1000)
            .OnValueChanged_Lambda([this](int x) {
                int const curr_min_value = (int)std::ceil(
                    params_.habitable_systems_per_hex_mean +
                    params_.habitable_systems_per_hex_plus_minus);
                if (x < curr_min_value)
                    x = curr_min_value;
                params_.systems_per_hex = x;
            })]
    ];

    vbox_->AddSlot().MinSize(50).AutoHeight()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_text_block)
            .Text(loc_text(TEXT("setup_galaxy_height")))]
        +SHorizontalBox::Slot().FillWidth(1)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_int_spin_box)
            .MinValue(11)
            .Value_Lambda([this]{ return params_.map_height;})
            .MaxValue(101)
            .OnValueChanged_Lambda([this](int x) {
                if (x == params_.map_height + 1)
                    ++x;
                else if (x == params_.map_height - 1)
                    --x;
                else if (x % 2 == 0)
                    ++x;
                params_.map_height = x;
            })]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
