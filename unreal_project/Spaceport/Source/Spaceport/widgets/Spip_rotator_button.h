#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <Widgets/SCompoundWidget.h>


class Stext_rotator;
class SImage;
class UMaterialInstanceDynamic;

class Spip_rotator_button : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Spip_rotator_button) {}
    SLATE_ARGUMENT(TArray<FText>, settings)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    // Set this when you don't want to do the rebind immediately, but instead
    // want to add the act of rebinding to a list of callbables, to be called
    // later (such as when the user hits the "Apply" button.
    void rebind_action_target(
        std::shared_ptr<std::vector<std::function<void()>>> target);

private:
    TSharedPtr<Stext_rotator> text_;
    TSharedPtr<SImage> pips_;
    std::shared_ptr<std::vector<std::function<void()>>> rebind_action_target_;
    FSlateBrush pip_brush_;
    UMaterialInstanceDynamic * pip_material_;
};
