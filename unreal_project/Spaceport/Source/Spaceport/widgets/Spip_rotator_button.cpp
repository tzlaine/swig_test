#include "Spip_rotator_button.h"
#include "Sstyled_button.h"
#include "Sstyled_text_block.h"
#include "game_instance.h"
#include "utility.hpp"

#include <ui_defaults.h>

#include <adobe/name.hpp>

#include <SlateMaterialBrush.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>
#include <Widgets/Images/SImage.h>


class Stext_rotator : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Stext_rotator) {}
    SLATE_ARGUMENT(TArray<FText>, texts)
    SLATE_END_ARGS()

    void Construct(FArguments const & args)
    {
        all_texts_ = args._texts;
        check(0 < all_texts_.Num());
        ChildSlot.VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)[SAssignNew(curr_text_, Sstyled_text_block)
                                     .Text(all_texts_[selected_])];
    }

    FText curr_text() const
    {
        check(curr_text_);
        if (!curr_text_)
            return FText{};
        return curr_text_->GetText();
    }
    int index() const { return selected_; }

    void select(int i)
    {
        selected_ = i;
        if (!curr_text_)
            return;
        if (i < 0 || all_texts_.Num() <= i)
            return;
        curr_text_->SetText(all_texts_[selected_]);
    }
    void shift_left()
    {
        check(!all_texts_.IsEmpty());
        if (all_texts_.IsEmpty())
            return;
        select((all_texts_.Num() + selected_ - 1) % all_texts_.Num());
    }
    void shift_right()
    {
        check(!all_texts_.IsEmpty());
        if (all_texts_.IsEmpty())
            return;
        select((selected_ + 1) % all_texts_.Num());
    }

private:
    TSharedPtr<Sstyled_text_block> curr_text_;
    TArray<FText> all_texts_;
    int selected_ = 0;
};

void Spip_rotator_button::Construct(FArguments const & args)
{
    int const size = args._settings.Num();

    auto * pc = player_controller_base();
    UMaterialInterface * base_material = pc->materials().rotator_pip_;
    pip_material_.Reset(
        UMaterialInstanceDynamic::Create(base_material, nullptr));
    pip_material_->SetScalarParameterValue(TEXT("num_pips"), size);
    pip_material_->SetScalarParameterValue(TEXT("curr_pip"), 0);
    pip_brush_.SetResourceObject(pip_material_.Get());

    UFont * title_font = ui_defaults().title_font_.Get();

    TSharedPtr<Sstyled_button> lbutton, rbutton;

    // clang-format off
    ChildSlot.VAlign(VAlign_Fill).HAlign(HAlign_Fill)[
        SNew(SHorizontalBox)

        +SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Center)[
            SAssignNew(lbutton, Sstyled_button)
            .OnClicked_Lambda([this] {
                text_->shift_left();
                pip_material_->SetScalarParameterValue(
                    TEXT("curr_pip"), text_->index());
                if (notifier_)
                    notifier_(text_->index());
                return FReply::Handled();
            })
        ]

        +SHorizontalBox::Slot().FillWidth(1).HAlign(HAlign_Fill)[
            SNew(SVerticalBox)

            +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                SAssignNew(text_, Stext_rotator)
                .texts(args._settings)
            ]

            +SVerticalBox::Slot()
            .MinHeight(40).MaxHeight(40).HAlign(HAlign_Fill)[
                SAssignNew(pips_, SImage).Image(&pip_brush_)
            ]
        ]

        +SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Center)[
            SAssignNew(rbutton, Sstyled_button)
            .OnClicked_Lambda([this] {
                text_->shift_right();
                pip_material_->SetScalarParameterValue(
                    TEXT("curr_pip"), text_->index());
                if (notifier_)
                    notifier_(text_->index());
                return FReply::Handled();
            })
        ]
    ];
    // clang-format on

    FSlateFontInfo big_font(title_font, ui_defaults().title_font_size_);
    lbutton->set_text(FText::FromString(TEXT("<")), &big_font);
    rbutton->set_text(FText::FromString(TEXT(">")), &big_font);
}

FText Spip_rotator_button::curr_text() const
{
    check(text_);
    return text_->curr_text();
}

int Spip_rotator_button::index() const
{
    check(text_);
    return text_->index();
}

void Spip_rotator_button::select(int i, bool notify)
{
    if (!text_)
        return;
    text_->select(i);
    pip_material_->SetScalarParameterValue(TEXT("curr_pip"), i);
    if (notifier_ && notify)
        notifier_(i);
}

void Spip_rotator_button::notifier(std::function<void(int)> notifier)
{
    notifier_ = std::move(notifier);
}
