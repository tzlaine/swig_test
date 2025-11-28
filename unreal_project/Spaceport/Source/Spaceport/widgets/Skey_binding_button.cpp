#include "Skey_binding_button.h"
#include "Sstyled_button.h"
#include "game_instance.h"
#include "utility.hpp"

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/SOverlay.h>


class key_listener : public SUserWidget
{
public:
    void Construct(FArguments const & args) { SUserWidget::Construct(args); }

    void listen(
        FName name,
        TSharedPtr<Sstyled_button> button,
        TSharedPtr<SWidget> prev_focus)
    {
        check(button);
        name_ = name;
        listening_for_ = button;
        prev_focus_ = prev_focus;
    }

    bool SupportsKeyboardFocus() const override { return true; }
    FReply OnKeyDown(FGeometry const & g, FKeyEvent const & e) override
    {
        if (listening_for_) {
            auto * pc = player_controller_base();
            check(pc);
            pc->remap_key(name_, e.GetKey());
            listening_for_->set_text(e.ToText());
            listening_for_.Reset();
            FSlateApplication::Get().SetKeyboardFocus(prev_focus_);
            prev_focus_.Reset();
            return FReply::Handled();
        } else {
            return SUserWidget::OnKeyDown(g, e);
        }
    }

    static TSharedRef<key_listener> New()
    {
        return MakeShareable(new key_listener());
    }

private:
    FName name_;
    TSharedPtr<Sstyled_button> listening_for_;
    TSharedPtr<SWidget> prev_focus_;
};

void Skey_binding_button::Construct(FArguments const & args)
{
    name_ = args._name;

    // clang-format off
    ChildSlot.VAlign(VAlign_Fill).HAlign(HAlign_Fill)[
        SNew(SOverlay)

        +SOverlay::Slot()[SAssignNew(key_listener_, key_listener)]

        +SOverlay::Slot()[
            SAssignNew(button_, Sstyled_button)
            .Text(args._key)
            .OnClicked_Lambda([this] {
                TSharedPtr<SWidget> prev_focus =
                    FSlateApplication::Get().GetUserFocusedWidget(0);
                key_listener_->listen(name_, button_, prev_focus);
                FSlateApplication::Get().SetKeyboardFocus(key_listener_);
                button_->set_text(loc_text(TEXT("press_any_key")));
                UE_LOG(LogTemp, Log, TEXT("CLICKED!"));
                return FReply::Handled();
            })
        ]
    ];
    // clang-format on
}
