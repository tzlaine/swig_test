#include "Smain_menu.h"
#include "game_instance.h"
#include "Amain_menu_controller.h"
#include "Aplayer_controller.h"
#include "Aplaying_hud.h"
#include "widgets/Sstyled_text_block.h"
#include "text/grapheme_break.hpp"
#include "text/beman_utf_view/utf_view.hpp"

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


using namespace adobe::literals;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

namespace {
    Aplaying_hud * playing_hud()
    {
        auto * pc = Cast<Aplayer_controller>(
            ::world()->GetFirstPlayerController());
        return Cast<Aplaying_hud>(pc->GetHUD());
    }
}

Smain_menu::Smain_menu()
{
    anims_.insert(
        "fadein"_name,
        animation(
            FString(TEXT("/Game/sigmoid_curve.sigmoid_curve")),
            0.1,
            [this](float value) { SetRenderOpacity(value); }));
}

void Smain_menu::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    in_game_ = args._in_game;

    TSharedPtr<SHorizontalBox> title_hbox;

    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 127)))]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.2, 0.2, 0.8, 0.2))
        .Offset(FMargin(0, 0, 0, 155))[
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot().FillWidth(5)

            +SHorizontalBox::Slot().FillWidth(90)[
                SAssignNew(title_hbox, SHorizontalBox)]

            +SHorizontalBox::Slot().FillWidth(5)
        ]

        +SConstraintCanvas::Slot()
        .Anchors(FAnchors(0.4, 0.45, 0.6, 0.8))
        [SAssignNew(vbox_, SVerticalBox)]
    ];

    rebuild();

    FString game_title = loc_text(TEXT("main_menu_game_title")).ToString();
    auto const * const title_first = &*game_title.begin();
    auto const * const title_last = title_first + game_title.Len();
    auto r = std::ranges::subrange(title_first, title_last) |
             beman::utf_view::to_utf32;
    auto cp_first = r.begin();
    auto const cp_last = r.end();
    while (cp_first != cp_last) {
        if (cp_first != r.begin())
            title_hbox->AddSlot().FillWidth(1);
        auto grapheme_last = text::next_grapheme_break(cp_first, cp_last);
        auto str = FString(
            grapheme_last.base() - cp_first.base(), cp_first.base());
        title_hbox->AddSlot().AutoWidth()[
            SNew(Sstyled_text_block)
            .Text(FText::FromString(str))
            .Font(FSlateFontInfo(title_font,
                                 ui_defaults().title_font_size_ * 2))];
        cp_first = grapheme_last;
        if (cp_first != cp_last)
            title_hbox->AddSlot().FillWidth(1);
    }

    SetRenderOpacity(in_game_ ? 0 : 1);
}

void Smain_menu::rebuild()
{
    vbox_->ClearChildren();

    vbox_->AddSlot().AutoHeight()[
        SAssignNew(continue_bn_, Sstyled_button)
        .Text(loc_text(TEXT("continue_game")))
        .OnClicked_Lambda([in_game = in_game_] {
            if (in_game) {
                if (auto * hud = playing_hud())
                    hud->escape_pressed();
            } else {
                // TODO: Load most recent save.
            }
            return FReply::Handled();
        })];

    vbox_->AddSlot().FillHeight(1);

    if (in_game_) {
        vbox_->AddSlot().AutoHeight()[
            SNew(Sstyled_button)
            .Text(loc_text(TEXT("save_game")))
            .OnClicked_Lambda([] {
                // TODO
                return FReply::Handled();
            })];
    } else {
        vbox_->AddSlot().AutoHeight()[
            SNew(Sstyled_button)
            .Text(loc_text(TEXT("new_game")))
            .OnClicked_Lambda([] {
                Amain_menu_controller * pc = Cast<Amain_menu_controller>(
                    ::world()->GetFirstPlayerController());
                pc->server_new_game(game_kind::sp, FFilePath());
                return FReply::Handled();
            })];
    }

    vbox_->AddSlot().FillHeight(1);

    vbox_->AddSlot().AutoHeight()[
        SAssignNew(load_game_bn_, Sstyled_button)
        .Text(loc_text(TEXT("load_game")))
        .OnClicked_Lambda([in_game = in_game_] {
            if (in_game) {
                // TODO
            }
            // TODO
            return FReply::Handled();
        })];

    vbox_->AddSlot().FillHeight(1);

    if (in_game_) {
        vbox_->AddSlot().AutoHeight()[
            SNew(Sstyled_button)
            .Text(loc_text(TEXT("exit_to_main_menu")))
            .OnClicked_Lambda([] {
                playing_hud()->do_after_confirming([]{
                    Aplayer_controller * pc = Cast<Aplayer_controller>(
                        ::world()->GetFirstPlayerController());
                    pc->server_quit_to_menu();
                });
                return FReply::Handled();
            })];
    } else {
        vbox_->AddSlot().AutoHeight()[
            SNew(Sstyled_button)
            .Text(loc_text(TEXT("multiplayer_game")))
            .OnClicked_Lambda([] {
                // TODO: open multiplayer setup hud
                return FReply::Handled();
            })];
    }

    vbox_->AddSlot().FillHeight(1);

    vbox_->AddSlot().AutoHeight()[
        SNew(Sstyled_button)
        .Text(loc_text(TEXT("options")))
        .OnClicked_Lambda([] {
            // TODO
            return FReply::Handled();
        })];

    vbox_->AddSlot().FillHeight(3);

    vbox_->AddSlot().AutoHeight()[
        SNew(Sstyled_button)
        .Text(loc_text(TEXT("exit_game")))
        .OnClicked_Lambda([in_game = in_game_] {
            // TODO: Do something different in mp, if this user is not the
            // host.
            if (in_game)
                playing_hud()->do_after_confirming([]{quit_game();});
            else
                quit_game();
            return FReply::Handled();
        })];
}

void Smain_menu::in_game(bool b)
{
    in_game_ = b;
    rebuild();
}

void Smain_menu::have_saves(bool b)
{
    if (in_game_)
        return;
    continue_bn_->SetEnabled(b);
}

bool Smain_menu::cancelable()
{
    return true;
}

void Smain_menu::show(UWorld * w)
{
    Shud_widget_base::show(w);
    anims_.start("fadein"_name);
}

void Smain_menu::Tick(FGeometry const & g, double t, float dt)
{
    Shud_widget_base::Tick(g, t, dt);
    if (!anims_.need_update())
        return;

    anims_.update(dt);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
