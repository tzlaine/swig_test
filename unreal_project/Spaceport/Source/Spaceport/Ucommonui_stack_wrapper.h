#pragma once

#include <CommonActivatableWidget.h>
#include <CoreMinimal.h>
#include <Blueprint/UserWidget.h>
#include <Widgets/CommonActivatableWidgetContainer.h>
#include "Ucommonui_stack_wrapper.generated.h"


UCLASS(Blueprintable)
class Ucommonui_stack_wrapper : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ui")
    TObjectPtr<UCommonActivatableWidgetStack> stack_;
};
