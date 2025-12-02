#include "Sgame_setup.h"
#include "Agame_mode.h"
#include "Aplayer_controller.h"
#include "game_instance.h"
#include "utility.hpp"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_spin_box.h"
#include "widgets/Sstyled_text_block.h"
#include <ui_defaults.h>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SConstraintCanvas.h>
#include <Widgets/Layout/SBackgroundBlur.h>
#include <Widgets/Text/STextBlock.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Sgame_setup::Construct(FArguments const & args)
{
    UFont * title_font = ui_defaults().title_font_.Get();

    TSharedPtr<SVerticalBox> vbox;

    int vertical_spacing = 15;
    // clang-format off
    ChildSlot[SNew(SBackgroundBlur).BlurStrength(5.0f)[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0, 1, 0.1))
        .Offset(FMargin(20, 20, 20, 0))[
            SNew(SVerticalBox)
            +SVerticalBox::Slot().AutoHeight()[
                SNew(Sstyled_text_block)
                .Text(loc_text(TEXT("game_setup_title")))
                .Font(FSlateFontInfo(title_font,
                                     ui_defaults().title_font_size_))
            ]
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0.1, 1, 0.9))
        .Offset(FMargin(20, 10, 20, 0))[
            SAssignNew(vbox, SVerticalBox)
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0, 0.9, 1, 1))
        .Offset(FMargin(20, 0, 20, 20))[
            SNew(SVerticalBox)

            +SVerticalBox::Slot().Padding(0, 10)[
                SNew(SHorizontalBox)

                +SHorizontalBox::Slot().FillWidth(50)

                +SHorizontalBox::Slot().FillWidth(25)[
                    SNew(Sstyled_button).Text(loc_text(TEXT("play")))
                    .OnClicked_Lambda([this] {
                        TArray<uint8> params = to_tarray(params_);
                        auto * pc = Cast<Aplayer_controller>(
                            ::world()->GetFirstPlayerController());
                        pc->server_start_game(params);
                        return FReply::Handled();
                    })
                ]

                +SHorizontalBox::Slot().FillWidth(25).Padding(10, 0)[
                    SNew(Sstyled_button).Text(loc_text(TEXT("back")))
                    .OnClicked_Lambda([this] {
                        back();
                        return FReply::Handled();
                    })
                ]
            ]
        ]
    ]];
    // clang-format on

    auto const adjust_total_for_inhab = [this] {
        int const curr_min_value = (int)std::ceil(
            params_.habitable_systems_per_hex_mean +
            params_.habitable_systems_per_hex_plus_minus);
        if (params_.systems_per_hex < curr_min_value)
            params_.systems_per_hex = curr_min_value;
    };

    vbox->AddSlot().MinSize(50).AutoHeight()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_text_block)
            .Text(loc_text(TEXT("setup_inhab_systems")))]
        +SHorizontalBox::Slot().FillWidth(100)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_float_spin_box)
            .MinValue(1.0)
            .Value_Lambda([this] {
                return params_.habitable_systems_per_hex_mean;})
            .MaxValue(50.0)
            .Delta(0.1)
            .MaxFractionalDigits(1)
            .OnValueChanged_Lambda([=, this](float x) {
                params_.habitable_systems_per_hex_mean = x;
                adjust_total_for_inhab();
            })]
        +SHorizontalBox::Slot().FillWidth(1)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_text_block)
            .Text(loc_text(TEXT("plus_minus_about")))]
        +SHorizontalBox::Slot().FillWidth(1)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_float_spin_box)
            .MinValue(1.0)
            .Value_Lambda([this] {
                return params_.habitable_systems_per_hex_plus_minus;})
            .MaxValue(10.0)
            .Delta(0.1)
            .MaxFractionalDigits(1)
            .OnValueChanged_Lambda([=, this](float x) {
                params_.habitable_systems_per_hex_plus_minus = x;
                adjust_total_for_inhab();
            })]
    ];

    vbox->AddSlot().MinSize(50).AutoHeight()[
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_text_block)
            .Text(loc_text(TEXT("setup_total_systems")))]
        +SHorizontalBox::Slot().FillWidth(1)
        +SHorizontalBox::Slot().AutoWidth()[
            SNew(Sstyled_int_spin_box)
            .MinValue(1)
            .Value_Lambda([=, this]{ return params_.systems_per_hex;})
            .MaxValue(1000)
            .OnValueChanged_Lambda([=, this](int x) {
                params_.systems_per_hex = x;
                adjust_total_for_inhab();
            })]
    ];

    vbox->AddSlot().MinSize(50).AutoHeight()[
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

bool Sgame_setup::cancelable()
{
    return true;
}

void Sgame_setup::cancel()
{
    back();
}

void Sgame_setup::back()
{
    auto * pc = Cast<Aplayer_controller>(
        ::world()->GetFirstPlayerController());
    pc->server_quit_to_menu();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
