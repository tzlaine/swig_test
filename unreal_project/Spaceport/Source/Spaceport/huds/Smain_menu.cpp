#include "Smain_menu.h"
#include "SlateOptMacros.h"

#include <Widgets/SUserWidget.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void Smain_menu::Construct(FArguments const & InArgs)
{
    in_game_ = InArgs._in_game;

    ChildSlot [ SAssignNew(vbox_, SVerticalBox) ];

    rebuild();
}

void Smain_menu::rebuild()
{
    vbox_->ClearChildren();
    for (int32 i = 0; i < 5; ++i) { // TODO
        vbox_->AddSlot()[SNew(SButton).Text(
            FText::FromString(FString::Printf(TEXT("%d"), i)))];
    }
}

void Smain_menu::in_game(bool b)
{
    in_game_ = b;
    rebuild();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
