#include "game_instance.h"


void Ugame_instance::load(level l)
{
    UGameplayStatics::OpenLevelBySoftObjectPtr(
        GetWorld(), l == level::start ? start_level_ : playing_level_);
}

void Ugame_instance::play_sound_across_level_loads(USoundBase * sound)
{
    audio_component_ =
        UGameplayStatics::SpawnSound2D(this, sound, 1, 1, 0, nullptr, true);
}
