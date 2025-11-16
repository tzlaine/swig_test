#pragma once

#include "model.hpp"

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>
#include "Agame_mode.generated.h"


template<typename T>
TArray<uint8> to_tarray(T const & x)
{
    auto as_protobuf = to_protobuf(x);
    TArray<uint8> buf;
    buf.SetNum(as_protobuf.ByteSize());
    as_protobuf.SerializeWithCachedSizesToArray(buf.GetData());
    return std::move(buf);
}

template<typename T>
T from_tarray(TArray<uint8> const & buf)
{
    decltype(to_protobuf(std::declval<T>())) as_protobuf;
    as_protobuf.ParseFromArray(buf.GetData(), buf.Num());
    return from_protobuf(as_protobuf);
}

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
class Agame_mode : public AGameModeBase
{
    GENERATED_BODY()

public:
    Agame_mode(FObjectInitializer const & init);

    virtual void BeginPlay() override;
    virtual void Tick(float secs) override;

    UFUNCTION(NetMulticast, Reliable)
    void multicast_quit_to_menu();
    void multicast_quit_to_menu_Implementation();

    // Sent loaded save game data iff params.IsEmpty(); generate a new galaxy
    // otherwise.
    UFUNCTION(NetMulticast, Reliable)
    void distribute_initial_game_state(TArray<uint8> const & params);
    void distribute_initial_game_state_Implementation(TArray<uint8> const & params);

private:
    Aplaying_hud * hud() const;

    void ready_for_sp_game();
    void ready_for_mp_game();

    void start_play();

    // generating the galaxy
    std::jthread generation_thread_;
    std::unique_ptr<concurrent_queue<int>> percent_complete_;
    std::atomic_bool generation_complete_ = false;

    // playing the game
    model model_;
};
