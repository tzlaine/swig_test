// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UInstancedStaticMeshComponent;

#include "visual_config_t.hpp"
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
    TSubclassOf<AActor> large_fleet_actor_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * interior_hex_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * edge_hex_mesh_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * planet_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * star5_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * star6_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * star8_mesh_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * mobile_base_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * battlestation_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * starbase_mesh_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * unit_square_mesh_;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UStaticMesh * unit_cube_mesh_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UMaterial * solid_color_mat_;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "hex_map")
    class UMaterial * text_mat_;

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
    void create_offmap_areas ();
    void instantiate_hexes ();
    void instantiate_hex (hex_coord_t hc);

    visual_config::visual_config_t visual_config_;
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
    instances_t planets_;
    instances_t star5s_;
    instances_t star6s_;
    instances_t star8s_;

    instances_t mobile_bases_;
    instances_t battlestations_;
    instances_t starbases_;

    instances_t offmap_panels_;
    instances_t offmap_borders_;
    boost::container::flat_map<int, UTextRenderComponent *> offmap_labels_;

    FTimerHandle instantiation_timer_;
    bool hexes_instantiated_;

    UTimelineComponent* cursor_indicator_move_timeline_;
    FOnTimelineFloat cursor_indicator_move_fn;
    FVector hover_indicator_from_;
    FVector hover_indicator_to_;

    bool showing_ui_;
};
