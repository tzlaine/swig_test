#pragma once

#include <ui_defaults.h>

#include <Widgets/Layout/SScrollBox.h>


// It has slots, which are not copyable, and so copying 'args' in Construct()
// and adding some does not work.
#define STYLED_SCROLL_BOX()                                                    \
    SNew(SScrollBox).ScrollBarStyle(ui_defaults().ScrollBar_style_.Get())

#define ASSIGN_STYLED_SCROLL_BOX(w)                                            \
    SAssignNew(w, SScrollBox)                                                  \
        .ScrollBarStyle(ui_defaults().ScrollBar_style_.Get())
