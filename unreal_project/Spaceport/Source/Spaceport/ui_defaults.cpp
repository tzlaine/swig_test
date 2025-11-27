#include "ui_defaults.h"


ui_defaults_t const & ui_defaults()
{
    static ui_defaults_t retval;
    static bool once = true;

    if (once) {
        retval.title_font_path_ =
            TEXT("/Game/ui/fonts/Lato-Bold_Font.Lato-Bold_Font");
        retval.title_font_size_ = 48;

        retval.font_path_ =
            TEXT("/Game/ui/fonts/Lato-Regular_Font.Lato-Regular_Font");
        retval.font_size_ = 28;

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

        once = false;
    }

    return retval;
}
