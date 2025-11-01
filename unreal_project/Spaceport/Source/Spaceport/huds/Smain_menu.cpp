#include "Smain_menu.h"
#include "game_instance.h"
#include "Amain_menu_controller.h"
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

void Smain_menu::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    in_game_ = args._in_game;

    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 127)))]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.2, 0.2, 0.8, 0.2))
        .Offset(FMargin(0, 0, 0, 154.654633))
        [SNew(STextBlock)
            .Font(title_font ? FSlateFontInfo(title_font, 96)
                  : FSlateFontInfo("", 96))
            .Text(NSLOCTEXT("strings", "main_menu_game_title",
                            "F    O    R    E    X"))
            .Justification(ETextJustify::Center)]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.4, 0.45, 0.6, 0.8))
        [SAssignNew(vbox_, SVerticalBox)]
    ];

    rebuild();
}

void Smain_menu::rebuild()
{
    vbox_->ClearChildren();

    vbox_->AddSlot().AutoHeight()[
        SAssignNew(continue_bn_, Sstyled_button)
        .Text(loc_text(TEXT("continue_game")))
        .OnClicked_Lambda([] {
            UE_LOG(LogTemp, Warning, TEXT("Continue"));
            return FReply::Handled();
        })];

    vbox_->AddSlot().FillHeight(1);

    if (in_game_) {
        vbox_->AddSlot().AutoHeight()[
            SAssignNew(new_game_save_game_bn_, Sstyled_button)
            .Text(loc_text(TEXT("save_game")))
            .OnClicked_Lambda([] {
                UE_LOG(LogTemp, Warning, TEXT("Save Game"));
                return FReply::Handled();
            })];
    } else {
        vbox_->AddSlot().AutoHeight()[
            SAssignNew(new_game_save_game_bn_, Sstyled_button)
            .Text(loc_text(TEXT("new_game")))
            .OnClicked_Lambda([] {
                UE_LOG(LogTemp, Warning, TEXT("New Game"));
                Amain_menu_controller * pc = Cast<Amain_menu_controller>(
                    UGameplayStatics::GetPlayerController(::world(), 0));
                pc->server_new_game(game_kind::sp, FFilePath());
                return FReply::Handled();
            })];
    }

    vbox_->AddSlot().FillHeight(1);

    vbox_->AddSlot().AutoHeight()[
        SAssignNew(load_game_bn_, Sstyled_button)
        .Text(loc_text(TEXT("load_game")))
        .OnClicked_Lambda([confirm = in_game_] {
            UE_LOG(LogTemp, Warning, TEXT("Load Game"));
            if (confirm) {
                // TODO: pop up a confirmation dialog
                // if (!...)
                //     return;
            }
            // TODO: load game
            return FReply::Handled();
        })];

    vbox_->AddSlot().FillHeight(1);

    if (in_game_) {
        vbox_->AddSlot().AutoHeight()[
            SNew(Sstyled_button)
            .Text(loc_text(TEXT("exit_to_main_menu")))
            .OnClicked_Lambda([] {
                UE_LOG(LogTemp, Warning, TEXT("Exit to main menu"));
                // TODO: pop up a confirmation dialog
                // if (!...)
                //     return;
                // TODO: end game, return to start_screen level
                return FReply::Handled();
            })];
    } else {
        vbox_->AddSlot().AutoHeight()[
            SNew(Sstyled_button)
            .Text(loc_text(TEXT("multiplayer_game")))
            .OnClicked_Lambda([] {
                UE_LOG(LogTemp, Warning, TEXT("Multiplayer Game"));
                // TODO: open multiplayer setup hud
                return FReply::Handled();
            })];
    }

    vbox_->AddSlot().FillHeight(1);

    vbox_->AddSlot().AutoHeight()[
        SNew(Sstyled_button)
        .Text(loc_text(TEXT("options")))
        .OnClicked_Lambda([] {
            UE_LOG(LogTemp, Warning, TEXT("Options"));
            return FReply::Handled();
        })];

    vbox_->AddSlot().FillHeight(3);

    vbox_->AddSlot().AutoHeight()[
        SNew(Sstyled_button)
        .Text(loc_text(TEXT("exit_game")))
        .OnClicked_Lambda([confirm = in_game_] {
            UE_LOG(LogTemp, Warning, TEXT("Outta here!"));
            // TODO: Do something different in mp, if this user is not the
            // host.
            if (confirm) {
                // TODO: pop up a confirmation dialog
                // if (!...)
                //     return;
            }
            quit_game();
            return FReply::Handled();
        })];
}

void Smain_menu::in_game(bool b)
{
    in_game_ = b;
    rebuild();
}

void Smain_menu::saves_available(bool b)
{
    continue_bn_->SetEnabled(b);
    load_game_bn_->SetEnabled(b);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
