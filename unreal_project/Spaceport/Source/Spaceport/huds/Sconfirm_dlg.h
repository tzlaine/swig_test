#pragma once

#include "Shud_widget_base.h"

#include <CoreMinimal.h>


class Sconfirm_dlg : public Shud_widget_base
{
public:
    enum struct result {
        waiting_for_user,
        yes,
        no
    };

    SLATE_BEGIN_ARGS(Sconfirm_dlg)
        {}
    SLATE_ARGUMENT(FString, title)
    SLATE_ARGUMENT(FString, message)
    SLATE_ARGUMENT(FString, yes_button)
    SLATE_ARGUMENT(FString, no_button)
    SLATE_ARGUMENT(result *, result_ptr)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

private:
    result * result_ptr_ = nullptr;
};
