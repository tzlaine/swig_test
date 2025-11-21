#pragma once

#include "Shud_widget_base.h"
#include "widgets/Sstyled_list_view.h"

#include <CoreMinimal.h>
#include <Widgets/Views/SListView.h>


class SEditableTextBox;
class Sstyled_button;
class Sstyled_editable_text_box;

class Ssave_load_dlg : public Shud_widget_base
{
public:
    using item_data = TSharedPtr<FString>;

    SLATE_BEGIN_ARGS(Ssave_load_dlg)
        {}
    SLATE_ARGUMENT(bool, saving)
    SLATE_ARGUMENT(bool, in_game)
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    bool cancelable() override;

    void saves_changed(TArray<FString> const & files);

private:
    bool filename_is_well_formed() const;
    bool filename_is_in_list() const;

    FString filename_;

    TSharedPtr<Sstyled_editable_text_box> edit_;
    TArray<item_data> items_;
    TSharedPtr<Sstyled_list_view<item_data>> list_box_;
    TSharedPtr<Sstyled_button> action_button_;

    bool saving_ = false;
    bool in_game_ = true;
};
