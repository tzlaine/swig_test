#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>
#include <Engine/Font.h>
#include "ui_defaults.generated.h"


UCLASS(BlueprintType, Blueprintable)
class Uui_defaults_t : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<UFont> title_font_;
    UPROPERTY(EditAnywhere, Category = "UI")
    int32 title_font_size_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<UFont> font_;
    UPROPERTY(EditAnywhere, Category = "UI")
    int32 font_size_;

    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> Button_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> CheckBox_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> ComboBox_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> EditableText_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> EditableTextBox_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> ProgressBar_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> SpinBox_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> ScrollBar_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> ScrollBox_style_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<USlateWidgetStyleAsset> TextBlock_style_;
};

Uui_defaults_t const & ui_defaults();
