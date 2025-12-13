#pragma once

#include "Amap_pawn_base.h"

#include <CoreMinimal.h>
#include "Amap_hex.generated.h"


class Agame_mode;
class Agame_mode_base;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class Amap_hex : public Amap_pawn_base
{
    GENERATED_BODY()

public:
    Amap_hex();

    void BeginPlay() override;
    void Tick(float delta) override;

    void select(bool b) override;
    void hover(bool b) override;
    map_pawn_kind kind() const override { return map_pawn_kind::hex; }

    int hex_id() { return hex_id_; }

private:
    void hex_id(int id) { hex_id_ = id; }

    int hex_id_ = hex_none;

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

    friend Agame_mode;
    friend Agame_mode_base;
};
