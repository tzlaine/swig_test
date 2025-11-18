#pragma once

#include <CoreMinimal.h>
#include <GameFramework/HUD.h>
#include "Aplaying_hud.generated.h"


class Smain_menu;
class Sgame_setup;
class Sgenerating_galaxy;

UCLASS()
class Aplaying_hud : public AHUD
{
    GENERATED_BODY()

public:
    Aplaying_hud(FObjectInitializer const & init);

    void have_saves(bool b);

    void toggle_main_menu();
    void show_main_menu();
    void hide_main_menu();

    void show_game_setup();
    void hide_game_setup();

    void show_generating_galaxy();
    void hide_generating_galaxy();
    void generating_percent_update(int p);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type reason) override;

    void allocate_widgets();

    TSharedPtr<Smain_menu> main_menu_;
    bool main_menu_up_ = false;
    TSharedPtr<Sgame_setup> game_setup_;
    TSharedPtr<Sgenerating_galaxy> generating_galaxy_;
    int generating_progress_ = 0;
};
