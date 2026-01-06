#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>
#include <Engine/Font.h>
#include <Styling/SlateBrush.h>
#include <Widgets/Views/SListView.h>
#include "ui_defaults.generated.h"


class UDataTable;

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

    UPROPERTY(EditAnywhere, Category = "UI")
    UDataTable * rich_text_styles_;

    // Brushes
    UPROPERTY(EditAnywhere, Category = "UI")
    FSlateBrush border_background_;

    // SListView rows
    FTableRowStyle table_row_style_;
    TSharedRef<ITableRow> make_row(
        TSharedPtr<FString> text,
        TSharedRef<STableViewBase> const & table) const;

    // Map
    UPROPERTY(EditAnywhere, Category = "UI")
    float camera_pan_speed_ = 1.0f;
    UPROPERTY(EditAnywhere, Category = "UI")
    float camera_zoom_speed_ = 1.0f;
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor drag_selection_box_color_ = FLinearColor(1, 1, 1, 1);
    UPROPERTY(EditAnywhere, Category = "UI")
    float map_scale_ = 100.0f;
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor system_selected_color_ = FLinearColor(1, 1, 1, 1);
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor system_hovered_color_ = FLinearColor(1, 1, 1, 0.5);

    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor speed_button_normal_color_ = FLinearColor(0.8, 0.8, 0.8, 1);
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor speed_button_hovered_color_ = FLinearColor(0.9, 0.9, 0.9, 1);
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor speed_button_pressed_color_ = FLinearColor(1, 1, 1, 1);
    UPROPERTY(EditAnywhere, Category = "UI")
    FLinearColor speed_button_disabled_color_ =
        FLinearColor(0.5, 0.5, 0.5, 0.5);
};

Uui_defaults_t const & ui_defaults();
