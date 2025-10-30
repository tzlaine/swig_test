#include "Agame_mode.h"
#include "Amap_hud.h"
#include "Aplayer_controller.h"


Agame_mode::Agame_mode(FObjectInitializer const & init) :
    AGameModeBase(init)
{
    HUDClass = Amap_hud::StaticClass();
    PlayerControllerClass = Aplayer_controller::StaticClass();
}

void Agame_mode::BeginPlay()
{
    Super::BeginPlay();

}
