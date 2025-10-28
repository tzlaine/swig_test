#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>


class Smain_menu : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(Smain_menu)
        {}
    SLATE_ARGUMENT(bool, in_game)
    SLATE_END_ARGS()

    void Construct(FArguments const & InArgs);

    void rebuild();
    void in_game(bool b);

private:
    TSharedPtr<SVerticalBox> vbox_;
    bool in_game_ = true;
};
