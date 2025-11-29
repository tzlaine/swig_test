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
        TSharedPtr<SWidget> prev_focus,
        FText const & prev_text)
    {
        check(button);
        name_ = name;
        listening_for_ = button;
        prev_focus_ = prev_focus;
        prev_text_ = prev_text;
    }

    void cancel_listen()
    {
        if (!listening_for_)
            return;
        listening_for_->set_text(prev_text_);
        listening_for_.Reset();
        prev_focus_.Reset();
    }

    void rebind_action_target(
        std::shared_ptr<std::vector<std::function<void()>>> target)
    {
        rebind_action_target_ = target;
    }

    bool SupportsKeyboardFocus() const override { return true; }
    void OnFocusLost(FFocusEvent const &) { cancel_listen(); }
    FReply OnKeyDown(FGeometry const & g, FKeyEvent const & e) override
    {
        if (listening_for_) {
            auto const do_rebind = [&] {
                auto * pc = player_controller_base();
                check(pc);
                pc->remap_key(name_, e.GetKey());
                listening_for_->set_text(
                    FText::FromString(e.GetKey().ToString()));
                listening_for_.Reset();
                FSlateApplication::Get().SetKeyboardFocus(prev_focus_);
                prev_focus_.Reset();
            };
            if (rebind_action_target_)
                rebind_action_target_->emplace_back(do_rebind);
            else
                do_rebind();
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
    FText prev_text_;
    std::shared_ptr<std::vector<std::function<void()>>> rebind_action_target_;
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
            .Text(FText::FromString(args._key))
            .OnClicked_Lambda([this] {
                TSharedPtr<SWidget> prev_focus =
                    FSlateApplication::Get().GetUserFocusedWidget(0);
                key_listener_->listen(name_, button_, prev_focus, button_->get_text());
                FSlateApplication::Get().SetKeyboardFocus(key_listener_);
                button_->set_text(loc_text(TEXT("press_any_key")));
                return FReply::Handled();
            })
        ]
    ];
    // clang-format on

    key_listener_->rebind_action_target(rebind_action_target_);
}

void Skey_binding_button::rebind_action_target(
    std::shared_ptr<std::vector<std::function<void()>>> target)
{
    rebind_action_target_ = target;
}

void Skey_binding_button::set_text(FText const & text)
{
    button_->set_text(text);
    key_listener_->cancel_listen();
}
