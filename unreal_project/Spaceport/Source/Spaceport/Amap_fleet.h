#pragma once

#include "Amap_pawn_base.h"
#include "constants.hpp"
#include "game_data.hpp"

#include <CoreMinimal.h>
#include "Amap_fleet.generated.h"


class Agame_mode;
class UFloatingPawnMovement;
class UStaticMeshComponent;

UCLASS()
class Amap_fleet : public Amap_pawn_base
{
    GENERATED_BODY()

public:
    Amap_fleet();

    void BeginPlay() override;
    void Tick(float delta) override;

    void select(bool b) override;
    void hover(bool b) override;
    map_pawn_kind kind() const override { return map_pawn_kind::fleet; }

    nation_and_object_id_t id() const { return {nation_id_, fleet_id_}; }

protected:
    void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;

private:
    void execute_map_move(float delta);
    void move_to_system(int system_id, FVector map_location);
    void id(nation_and_object_id_t nao_id)
    {
        nation_id_ = nao_id.nation_id;
        fleet_id_ = nao_id.object_id;
    }

    UPROPERTY(ReplicatedUsing = OnRep_move_target)
    FVector move_to_map_location_ = FVector();
    UPROPERTY(ReplicatedUsing = OnRep_move_target)
    int32 move_to_system_ = system_none;

    UFUNCTION()
    void OnRep_move_target();

    UPROPERTY(ReplicatedUsing = OnRep_initial_properties)
    int32 nation_id_ = nation_none;
    UPROPERTY(ReplicatedUsing = OnRep_initial_properties)
    int32 fleet_id_ = object_none;

    UFUNCTION()
    void OnRep_initial_properties();

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> root_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> hit_mesh_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFloatingPawnMovement> movement_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> mesh_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> hover_indicator_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> selection_indicator_;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Map movement",
        meta = (AllowPrivateAccess = "true"))
    float map_move_eps_ = 0.01f;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Map movement",
        meta = (AllowPrivateAccess = "true"))
    float map_rotate_speed_ = 1.0f;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Map movement",
        meta = (AllowPrivateAccess = "true"))
    float map_rotate_eps_ = 0.01f;

    friend Agame_mode;
};
