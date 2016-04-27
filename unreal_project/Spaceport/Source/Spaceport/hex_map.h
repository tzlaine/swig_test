// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "model.hpp"
#include <vector>
#include "GameFramework/Actor.h"
#include "hex_map.generated.h"

UCLASS()
class SPACEPORT_API Ahex_map : public AActor
{
    GENERATED_BODY()

public:
    Ahex_map ();

    virtual void BeginPlay () override;

    virtual void Tick (float delta_seconds) override;

    hex_coord_t hex_under_cursor () const;

    UFUNCTION(BlueprintCallable, Category = "Hex")
    void hex_under_cursor (int & x, int & y) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    class UStaticMeshComponent * static_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    class APlayerController * player_controller_;

private:
    void spawn_hexes ();
    void spawn_hex (struct hex_t const & map_hex, int map_width, int map_height, UWorld* const world);

    start_data_t start_data_;
    std::vector<FLinearColor> nation_id_primary_colors_;
    std::vector<FLinearColor> nation_id_secondary_colors_;
    std::vector<class UInstancedStaticMeshComponent *> instanced_hexes_;
    FTimerHandle spawn_timer_;
    bool hexes_spawned;
};
