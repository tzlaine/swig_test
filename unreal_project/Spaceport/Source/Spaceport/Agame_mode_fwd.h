#pragma once

#include <CoreMinimal.h>
#include <Agame_mode_fwd.generated.h>


UENUM(BlueprintType)
enum class play_state : uint8 {
    setup,            // SP, or game host in MP
    waiting_for_game, // non-hosts in MP
    generating,
    playing,
    paused,
    ended
};

inline bool playing_or_paused(play_state s)
{
    return s == play_state::playing || s == play_state::paused;
}
