#pragma once

#include "Shud_widget_base.h"

#include <functional>

#include <CoreMinimal.h>
#include <CommonActivatableWidget.h>
#include <Widgets/SOverlay.h>
#include <Uactivatable_widget.generated.h>


// Wraps a Shud_widget_base, so that it can be used with the CommonUI
// framework.

UCLASS()
class Uactivatable_widget : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    Uactivatable_widget(FObjectInitializer const & init);

    bool wraps(Shud_widget_base const & w) const;
    bool cancelable() const;

    void content(
        std::function<TSharedPtr<Shud_widget_base>()> make_slate_widget);
    void cancel();

    void ReleaseSlateResources(bool b) override;
    TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<SOverlay> overlay_;
    TSharedPtr<Shud_widget_base> hud_widget_;
};
