#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>
#include <Engine/Font.h>
#include <Styling/SlateBrush.h>
#include <Widgets/Views/SListView.h>
#include "ui_defaults.generated.h"


UCLASS(BlueprintType, Blueprintable)
class Uui_defaults_t : public UObject
{
    GENERATED_BODY()

public:
    Uui_defaults_t();

    // Fonts
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<UFont> title_font_;
    UPROPERTY(EditAnywhere, Category = "UI")
    int32 title_font_size_;
    UPROPERTY(EditAnywhere, Category = "UI")
    TObjectPtr<UFont> font_;
    UPROPERTY(EditAnywhere, Category = "UI")
    int32 font_size_;

    // Slate styles
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
    UPROPERTY(EditAnywhere, Category = "UI")
    FSliderStyle Slider_style_;

    // Brushes
    UPROPERTY(EditAnywhere, Category = "UI")
    FSlateBrush border_background_;

    // SListView rows
    FTableRowStyle table_row_style_;
    TSharedRef<ITableRow> make_row(
        TSharedPtr<FString> text,
        TSharedRef<STableViewBase> const & table) const;

    // Miscellaneous
    UPROPERTY(EditAnywhere, Category = "UI")
    float camera_pan_speed_ = 1.0f;
    UPROPERTY(EditAnywhere, Category = "UI")
    float camera_zoom_speed_ = 1.0f;
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor drag_selection_box_color_ = FLinearColor(1, 1, 1, 1);
};

Uui_defaults_t const & ui_defaults();
