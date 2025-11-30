#include "Spip_rotator_button.h"
#include "Sstyled_button.h"
#include "Sstyled_text_block.h"
#include "game_instance.h"
#include "utility.hpp"

#include <ui_defaults.h>
#include <ui_util.h>

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
        selected_ = 0;
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
        check(!all_texts_.IsEmpty());
        check(0 <= i);
        check(i < all_texts_.Num());
        check(curr_text_);
        if (all_texts_.IsEmpty())
            return;
        if (!curr_text_)
            return;
        selected_ = i;
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
    int selected_ = -1;
};

void Spip_rotator_button::Construct(FArguments const & args)
{
    int const size = args._settings.Num();

    auto * pc = player_controller_base();
    UMaterialInterface * base_material = pc->materials().get(
        TEXT("/Game/ui/settings/rotator_pip_material.rotator_pip_material"));
    pip_material_ = UMaterialInstanceDynamic::Create(base_material, pc);
    pip_material_->SetScalarParameterValue(TEXT("num_pips"), size);
    pip_material_->SetScalarParameterValue(TEXT("curr_pip"), 0);
    pip_brush_.SetResourceObject(pip_material_);

    // clang-format off
    ChildSlot.VAlign(VAlign_Fill).HAlign(HAlign_Fill)[
        SNew(SHorizontalBox)

        +SHorizontalBox::Slot().FillWidth(20).MinWidth(50).HAlign(HAlign_Center)[
            SNew(Sstyled_button)
            .Text(FText::FromString(TEXT("<")))
            .OnClicked_Lambda([this] {
                text_->shift_left();
                pip_material_->SetScalarParameterValue(
                    TEXT("curr_pip"), text_->index());
                return FReply::Handled();
            })
        ]

        +SHorizontalBox::Slot().FillWidth(60).HAlign(HAlign_Fill)[
            SNew(SVerticalBox)

            +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                SAssignNew(text_, Stext_rotator)
                .texts(args._settings)
            ]

            +SVerticalBox::Slot().MinHeight(20).HAlign(HAlign_Fill)[
                SAssignNew(pips_, SImage).Image(&pip_brush_)
            ]
        ]

        +SHorizontalBox::Slot().FillWidth(20).MinWidth(50).HAlign(HAlign_Center)[
            SNew(Sstyled_button)
            .Text(FText::FromString(TEXT(">")))
            .OnClicked_Lambda([this] {
                text_->shift_right();
                pip_material_->SetScalarParameterValue(
                    TEXT("curr_pip"), text_->index());
                return FReply::Handled();
            })
        ]
    ];
    // clang-format on
}

void Spip_rotator_button::rebind_action_target(
    std::shared_ptr<std::vector<std::function<void()>>> target)
{
    rebind_action_target_ = target;
}

void Spip_rotator_button::select(int i)
{
    if (!text_)
        return;
    text_->select(i);
    pip_material_->SetScalarParameterValue(TEXT("curr_pip"), i);
}
