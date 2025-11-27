#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>


struct ui_defaults_t
{
    FString title_font_path_;
    int32 title_font_size_;

    FString font_path_;
    int32 font_size_;

    FString UButton_style_path_;
    FString UCheckBox_style_path_;
    FString UComboBox_style_path_;
    FString UEditableText_style_path_;
    FString UEditableTextBox_style_path_;
    FString UProgressBar_style_path_;
    FString USpinBox_style_path_;
    FString UScrollBar_style_path_;
    FString UScrollBox_style_path_;
    FString UTextBlock_style_path_;
};

ui_defaults_t const & ui_defaults();
