#pragma once

#include "Agame_mode_base.h"

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode.generated.h"


class Agame_state;
class Aplaying_hud;

UENUM(BlueprintType)
enum class play_state : uint8 {
    setup,            // SP, or game host in MP
    waiting_for_game, // non-hosts in MP
    generating,
    playing,
    paused,
    ended
};

UCLASS()
class Agame_mode : public Agame_mode_base
{
    GENERATED_BODY()

public:
    Agame_mode(FObjectInitializer const & init);

    void BeginPlay() override;
    void Tick(float secs) override;

    UFUNCTION(NetMulticast, Reliable)
    void multicast_quit_to_menu();
    void multicast_quit_to_menu_Implementation();

    // Sent loaded save game data iff params.IsEmpty(); generate a new galaxy
    // otherwise.
    UFUNCTION(NetMulticast, Reliable)
    void distribute_initial_game_state(TArray<uint8> const & params);
    void distribute_initial_game_state_Implementation(TArray<uint8> const & params);

    void save_game(FString const & filename);

private:
    Aplaying_hud * hud() const;

    void ready_for_sp_game();
    void ready_for_mp_game();
    void start_play();

    // generating the galaxy
    std::jthread generation_thread_;
    std::unique_ptr<concurrent_queue<int>> percent_complete_;
    std::atomic_bool generation_complete_ = false;
};
