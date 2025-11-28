#pragma once

#include <Widgets/SCompoundWidget.h>


class Sstyled_button;
class key_listener;

class Skey_binding_button : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Skey_binding_button)
        {}
    SLATE_ARGUMENT(FName, name)
    SLATE_ARGUMENT(FText, key)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    bool SupportsKeyboardFocus() const override { return true; }
    FReply OnKeyDown(FGeometry const & g, FKeyEvent const & e) override;

private:
    FName name_;
    TSharedPtr<Sstyled_button> button_;
    TSharedPtr<key_listener> key_listener_;
};
