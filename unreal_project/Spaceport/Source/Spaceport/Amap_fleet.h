#pragma once

#include "Amap_pawn_base.h"
#include "Agame_mode.h"
#include "constants.hpp"

#include <CoreMinimal.h>
#include "Amap_fleet.generated.h"


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

    nation_and_object_id_t nation_and_object_id() const
    {
        return nation_and_object_id_;
    }

    void move_to_system(int system_id, FVector map_location);

private:
    void execute_map_move(float delta);
    void id(nation_and_object_id_t nao_id) { nation_and_object_id_ = nao_id; }

    FVector move_to_map_location_;
    int move_to_system_ = system_none;
    nation_and_object_id_t nation_and_object_id_{nation_none, object_none};

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
