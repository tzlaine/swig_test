#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>
#include <Widgets/SBoxPanel.h>


class Shud_widget_base : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Shud_widget_base)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    virtual bool cancelable();
    virtual void cancel();
    virtual void show();
    virtual void hide();
};
