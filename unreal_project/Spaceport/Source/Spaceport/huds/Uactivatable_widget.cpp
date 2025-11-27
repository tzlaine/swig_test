#include "Uactivatable_widget.h"

#include <Widgets/Layout/SSpacer.h>


Uactivatable_widget::Uactivatable_widget(FObjectInitializer const & init) :
    Super(init)
{
    bIsBackHandler = false;
}

bool Uactivatable_widget::wraps(Shud_widget_base const & w) const
{
    return &w == hud_widget_.Get();
}

bool Uactivatable_widget::cancelable() const
{
    check(hud_widget_);
    return hud_widget_->cancelable();
}

void Uactivatable_widget::content(
    std::function<TSharedPtr<Shud_widget_base>()> make_slate_widget)
{
    check(make_slate_widget);
    hud_widget_ = make_slate_widget();
    overlay_->AddSlot()
        .VAlign(VAlign_Fill)
        .HAlign(HAlign_Fill)[hud_widget_.ToSharedRef()];
    InvalidateLayoutAndVolatility();
}

void Uactivatable_widget::cancel()
{
    check(hud_widget_);
    hud_widget_->cancel();
}

void Uactivatable_widget::ReleaseSlateResources(bool b)
{
    Super::ReleaseSlateResources(b);
    hud_widget_.Reset();
    overlay_.Reset();
}

TSharedRef<SWidget> Uactivatable_widget::RebuildWidget()
{
    overlay_ = SNew(SOverlay);
    return overlay_.ToSharedRef();
}
