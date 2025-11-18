#include "Aplaying_hud.h"
#include "game_instance.h"
#include "huds/Smain_menu.h"
#include "huds/Sgame_setup.h"
#include "huds/Sgenerating_galaxy.h"
#include "utility.hpp"

#include <Widgets/SViewport.h>


namespace {
    template<typename T>
    void show(UWorld * w, TSharedPtr<T> & widget)
    {
        if (auto * viewport = w->GetGameViewport())
            viewport->AddViewportWidgetContent(widget.ToSharedRef());
    }
    template<typename T>
    void hide(UWorld * w, TSharedPtr<T> & widget)
    {
        if (auto * viewport = w->GetGameViewport())
            viewport->RemoveViewportWidgetContent(widget.ToSharedRef());
    }
}

Aplaying_hud::Aplaying_hud(FObjectInitializer const & init) : AHUD(init) {}

void Aplaying_hud::saves_available(bool b)
{
    have_saves_ = b;
    if (main_menu_)
        main_menu_->saves_available(b);
}

void Aplaying_hud::toggle_main_menu()
{
    if (main_menu_up_)
        hide_main_menu();
    else
        show_main_menu();
    main_menu_up_ = !main_menu_up_;
}
void Aplaying_hud::show_main_menu()
{
    allocate_widgets();
    show(GetWorld(), main_menu_);
    TArray<FString> saves = find_save_files();
    saves_available(!saves.IsEmpty());
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

void Aplaying_hud::show_generating_galaxy()
{
    allocate_widgets();
    show(GetWorld(), generating_galaxy_);
    generating_progress_ = 0;
}
void Aplaying_hud::hide_generating_galaxy()
{
    allocate_widgets();
    hide(GetWorld(), generating_galaxy_);
}
void Aplaying_hud::generating_percent_update(int u)
{
    if (!generating_galaxy_)
        return;
    generating_progress_ += u;
    generating_galaxy_->percent_complete(generating_progress_);
}

void Aplaying_hud::BeginPlay()
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplaying_hud::BeginPlay()"));
    Super::BeginPlay();

    allocate_widgets();
    main_menu_->saves_available(have_saves_);
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplaying_hud::BeginPlay()"));
}

void Aplaying_hud::EndPlay(EEndPlayReason::Type reason)
{
    UE_LOG(LogTemp, Log, TEXT("ENTER Aplaying_hud::EndPlay()"));
    Super::EndPlay(reason);

    hide_main_menu();
    hide_game_setup();
    hide_generating_galaxy();
    UE_LOG(LogTemp, Log, TEXT("EXIT Aplaying_hud::EndPlay()"));
}

void Aplaying_hud::allocate_widgets()
{
    if (main_menu_)
        return;
    main_menu_ = SNew(Smain_menu).in_game(true);
    game_setup_ = SNew(Sgame_setup);
    generating_galaxy_ = SNew(Sgenerating_galaxy);
}
