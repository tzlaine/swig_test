#pragma once


struct ui_defaults_t
{
    FString font_path_;

    FString UButton_style_path_;
    FString UCheckBox_style_path_;
    FString UComboBox_style_path_;
    FString USpinBox_style_path_;
    FString UTextBlock_style_path_;
};

#ifndef USER_DEFINED_UI_DEFAULTS

inline ui_defaults_t const & ui_defaults()
{
    static ui_defaults_t retval;
    static bool once = true;

    if (once) {
        retval.font_path_ = TEXT("/Game/ui/fonts/JosefinSans-Light");

        retval.UButton_style_path_ = TEXT("/Game/ui/styles/button_style");
        retval.UCheckBox_style_path_ = TEXT("/Game/ui/styles/check_box_style");
        retval.UComboBox_style_path_ = TEXT("/Game/ui/styles/combo_box_style");
        retval.USpinBox_style_path_ = TEXT("/Game/ui/styles/spin_box_style");
        retval.UTextBlock_style_path_ = TEXT("/Game/ui/styles/text_block_style");

        once = false;
    }

    return retval;
}

#endif
