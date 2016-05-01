// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UInstancedStaticMeshComponent;

#include "start_data_t.hpp"
#include "game_data_t.hpp"
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

    UFUNCTION(BlueprintCallable, Category = "hex_map")
    void hex_under_cursor (int & x, int & y) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * hover_indicator_mesh_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * interior_hex_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * edge_hex_mesh_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * planet_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * star_5_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * star_6_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * star_8_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UMaterial * solid_color_mat_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * hex_border_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UMaterial * hex_border_mat_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UMaterial * thin_hex_border_mat_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class APlayerController * player_controller_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    UCurveFloat* unit_curve_;
    UFUNCTION()
    void cursor_indicator_move_callback (float x);

private:
    using national_instances_t = std::vector<UInstancedStaticMeshComponent *>; // indexed by nation_id
    using color_instances_t = boost::container::flat_map<FColor, UInstancedStaticMeshComponent *>;

    struct instances_t
    {
        void use (UStaticMesh * mesh);
        void add (int nation_id, FTransform transform);

        national_instances_t instances_;
        color_instances_t by_color_;
    };

    void initialize (instances_t & instances, int nation_id, FColor color, std::string const & name);
    void initialize_border_instanced_mesh (instances_t & instances, FColor color, float thickness, UMaterial * mat);
    void use_solid_color (UStaticMeshComponent * instanced, FColor color);
    void spawn_hexes ();
    void spawn_hex (hex_coord_t hc);

    friend bool operator< (FColor lhs, FColor rhs)
    { return lhs.DWColor() < rhs.DWColor(); }

    start_data::start_data_t start_data_;
    game_data_t game_data_;

    std::vector<FColor> nation_id_primary_colors_;
    std::vector<FColor> nation_id_secondary_colors_;

    boost::container::flat_map<FColor, UMaterialInstanceDynamic *> solid_color_materials_;

    UStaticMeshComponent * hover_indicator_;

    instances_t interior_hexes_;
    instances_t edge_hexes_;
    instances_t national_borders_;
    instances_t province_borders_;
    instances_t hex_borders_;

    FTimerHandle spawn_timer_;
    bool hexes_spawned_;

    UTimelineComponent* cursor_indicator_move_timeline_;
    FOnTimelineFloat cursor_indicator_move_fn;
    FVector hover_indicator_from_;
    FVector hover_indicator_to_;
};
