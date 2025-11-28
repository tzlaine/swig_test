#pragma once

#include <functional>
#include <vector>

#include <Widgets/SCompoundWidget.h>


class Sstyled_button;
class key_listener;

class Skey_binding_button : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Skey_binding_button)
        {}
    SLATE_ARGUMENT(FName, name)
    SLATE_ARGUMENT(FString, key)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    // Set this when you don't want to do the rebind immediately, but instead
    // want to add the act of rebinding to a list of callbables, to be called
    // later (such as when the user hits the "Apply" button.
    void rebind_action_target(std::vector<std::function<void()>> & target);

    bool SupportsKeyboardFocus() const override { return true; }

private:
    FName name_;
    TSharedPtr<Sstyled_button> button_;
    TSharedPtr<key_listener> key_listener_;
    std::vector<std::function<void()>> * rebind_action_target_ = nullptr;
};
