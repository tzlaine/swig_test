#pragma once

#include <ui_defaults.h>
#include <ui_util.h>

#include <Widgets/Input/SButton.h>

#include <functional>


class Sstyled_button : public SButton
{
public:
    using FArguments = SButton::FArguments;

    void Construct(FArguments const & args);

    template<typename Fn>
    void connect(Fn && fn)
    { fn_ = fn; }

private:
    FReply trampoline();

    std::function<void ()> fn_;
};
