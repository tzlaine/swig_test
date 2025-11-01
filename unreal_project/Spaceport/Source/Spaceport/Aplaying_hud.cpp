#include "Aplaying_hud.h"
#include "game_instance.h"
#include "huds/Smain_menu.h"
#include "huds/Sgame_setup.h"

#include <Widgets/SViewport.h>


namespace {
    template<typename T>
    void show(UWorld * w, TSharedPtr<T> & widget)
    {
        w->GetGameViewport()->AddViewportWidgetContent(
            widget.ToSharedRef());
    }
    template<typename T>
    void hide(UWorld * w, TSharedPtr<T> & widget)
    {
        w->GetGameViewport()->RemoveViewportWidgetContent(
            widget.ToSharedRef());
    }
}

Aplaying_hud::Aplaying_hud(FObjectInitializer const & init) : AHUD(init) {}


void Aplaying_hud::saves_available(bool b)
{
    have_saves_ = b;
    if (main_menu_)
        main_menu_->saves_available(b);
}

void Aplaying_hud::show_main_menu()
{
    allocate_widgets();
    show(GetWorld(), main_menu_);
}
void Aplaying_hud::hide_main_menu()
{
    allocate_widgets();
    hide(GetWorld(), main_menu_);
}

void Aplaying_hud::show_game_setup()
{
    allocate_widgets();
    show(GetWorld(), game_setup_);
}
void Aplaying_hud::hide_game_setup()
{
    allocate_widgets();
    hide(GetWorld(), game_setup_);
}

void Aplaying_hud::BeginPlay()
{
    Super::BeginPlay();

    allocate_widgets();
    main_menu_->saves_available(have_saves_);
}

void Aplaying_hud::EndPlay(EEndPlayReason::Type reason)
{
    Super::EndPlay(reason);

    hide_main_menu();
    hide_game_setup();
}

void Aplaying_hud::allocate_widgets()
{
    if (main_menu_)
        return;
    main_menu_ = SNew(Smain_menu).in_game(true);
    game_setup_ = SNew(Sgame_setup);
}
