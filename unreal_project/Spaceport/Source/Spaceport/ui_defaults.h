#pragma once


struct ui_defaults_t
{
    FString font_path_;

    FString UButton_style_path_;
    FString UTextBlock_style_path_;

// TODO
//    FString UButton_class_path_;
//    FString UTextBlock_class_path_;
};

#ifndef USER_DEFINED_UI_DEFAULTS

inline ui_defaults_t const & ui_defaults()
{
    static ui_defaults_t retval;
    static bool once = true;

    if (once) {
        retval.font_path_ = TEXT("/Game/ui/fonts/JosefinSans-Light");

        retval.UButton_style_path_ = TEXT("/Game/ui/styles/button_style");
        retval.UTextBlock_style_path_ = TEXT("/Game/ui/styles/text_block_style");

//        retval.UButton_class_path_ = TEXT("/Script/Spaceport.styleable_button");
//        retval.UTextBlock_class_path_ = TEXT("/Script/Spaceport.styleable_text_block");

        once = false;
    }

    return retval;
}

#endif
