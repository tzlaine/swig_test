#pragma once

#include "Shud_widget_base.h"

#include <functional>
#include <list>

#include <CoreMinimal.h>


class Stab_panel;
class Sstyled_button;

class Soptions : public Shud_widget_base
{
public:
    SLATE_BEGIN_ARGS(Soptions)
        {}
    SLATE_END_ARGS()

    void Construct(FArguments const & args);

    bool cancelable() override;

private:
    struct panel_info
    {
        std::function<void()> apply_changes_;
        std::function<void()> restore_defaults_;
        bool can_apply_ = true;
    };

    panel_info const & get_panel_info(int i) const;
    panel_info const & curr_panel_info() const;

    TSharedPtr<Stab_panel> tab_panel_;
    TSharedPtr<Sstyled_button> apply_button_;
    std::list<panel_info> panel_infos_;
};
