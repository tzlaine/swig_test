#pragma once

#include <functional>
#include <ui_defaults.h>
#include <ui_util.h>
#include "CoreMinimal.h"
#include "Components/SpinBox.h"
#include "styled_spin_box.generated.h"

UCLASS()
class SPACEPORT_API Ustyled_spin_box : public USpinBox
{
    GENERATED_BODY()

    STYLEABLE_WIDGET_PUBLIC();

    Ustyled_spin_box ();

    virtual void OnWidgetRebuilt() override;

protected:
    STYLEABLE_WIDGET_PRIVATE();
};
