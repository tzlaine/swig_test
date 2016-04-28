// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UInstancedStaticMeshComponent;

#include "start_data_t.hpp"
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
    class UStaticMesh * hex_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    class UMaterial * hex_mat_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    class UStaticMesh * hex_border_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    class UMaterial * hex_border_mat_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    class APlayerController * player_controller_;

private:
    using national_instances_t = std::vector<UInstancedStaticMeshComponent *>; // indexed by nation_id

    void initialize_border_instanced_mesh (national_instances_t & instanced_meshes, int nation_id, float thickness);
    void spawn_hexes ();
    void spawn_hex (hex_coord_t hc);

    start_data_t start_data_;
    std::vector<FLinearColor> nation_id_primary_colors_;
    std::vector<FLinearColor> nation_id_secondary_colors_;
    national_instances_t instanced_hexes_;
    national_instances_t instanced_national_borders_;
    national_instances_t instanced_province_borders_;
    national_instances_t instanced_hex_borders_;
    FTimerHandle spawn_timer_;
    bool hexes_spawned_;
};
