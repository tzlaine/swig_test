#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>


struct ui_defaults_t
{
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

    FSlateColor UComboBoxString_USpinBox_foreground_color_;
};

#ifndef USER_DEFINED_UI_DEFAULTS

inline ui_defaults_t const & ui_defaults()
{
    static ui_defaults_t retval;
    static bool once = true;

    if (once) {
        retval.font_path_ = TEXT("/Game/ui/fonts/futura_light_bt_Font");
        retval.font_size_ = 24;

        retval.UButton_style_path_ = TEXT("/Game/ui/styles/button_style");
        retval.UCheckBox_style_path_ = TEXT("/Game/ui/styles/check_box_style");
        retval.UComboBox_style_path_ = TEXT("/Game/ui/styles/combo_box_style");
        retval.UEditableText_style_path_ = TEXT("/Game/ui/styles/editable_text_style");
        retval.UEditableTextBox_style_path_ = TEXT("/Game/ui/styles/editable_text_box_style");
        retval.UProgressBar_style_path_ = TEXT("/Game/ui/styles/progress_style");
        retval.USpinBox_style_path_ = TEXT("/Game/ui/styles/spin_box_style");
        retval.UScrollBar_style_path_ = TEXT("/Game/ui/styles/scroll_bar_style");
        retval.UScrollBox_style_path_ = TEXT("/Game/ui/styles/scroll_box_style");
        retval.UTextBlock_style_path_ = TEXT("/Game/ui/styles/text_block_style");

        retval.UComboBoxString_USpinBox_foreground_color_ = FLinearColor::White;

        once = false;
    }

    return retval;
}

#endif
