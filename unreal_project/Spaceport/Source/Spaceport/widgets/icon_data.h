#pragma once

#include <CommonUIRichTextData.h>
#include "icon_data.generated.h"


USTRUCT()
struct Ficon_data : public FRichTextIconData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = Appearance)
    FLinearColor color_and_opacity_;
};
