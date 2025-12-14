#include "user_notification.h"


Fuser_notification::Fuser_notification(
    FString title, FString message, FString button) :
    title_(std::move(title)),
    message_(FText::FromString(message)),
    button_(std::move(button))
{}

Fuser_notification::Fuser_notification(
    FString title, FText message, FString button) :
    title_(std::move(title)),
    message_(std::move(message)),
    button_(std::move(button))
{}
