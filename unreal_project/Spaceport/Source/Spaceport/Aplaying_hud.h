#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Aplaying_hud.generated.h"


class Smain_menu;
class Sgame_setup;

UCLASS()
class Aplaying_hud : public AHUD
{
    GENERATED_BODY()

public:
    Aplaying_hud(FObjectInitializer const & init);

    void saves_available(bool b);

    void show_main_menu();
    void hide_main_menu();

    void show_game_setup();
    void hide_game_setup();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type reason) override;

    void allocate_widgets();

    bool have_saves_ = false;

    TSharedPtr<Smain_menu> main_menu_;
    TSharedPtr<Sgame_setup> game_setup_;
};
