#pragma once

#include <CoreMinimal.h>
#include "user_notification.generated.h"


USTRUCT()
struct Fuser_notification
{
    GENERATED_BODY()

    Fuser_notification() = default;
    Fuser_notification(
        FString title, FString message, FString button = TEXT("ok"));
    Fuser_notification(
        FString title, FText message, FString button = TEXT("ok"));

    UPROPERTY()
    FString title_;
    UPROPERTY()
    FText message_;
    UPROPERTY()
    FString button_;
};
