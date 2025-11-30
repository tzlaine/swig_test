#pragma once

#include <functional>
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

    FText curr_text() const;
    int index() const;

    void select(int i, bool notify = true);
    void notifier(std::function<void(int)> notifier);

private:
    TSharedPtr<Stext_rotator> text_;
    TSharedPtr<SImage> pips_;
    FSlateBrush pip_brush_;
    TStrongObjectPtr<UMaterialInstanceDynamic> pip_material_;
    std::function<void(int)> notifier_;
};
