#pragma once

#include "Amap_pawn_base.h"
#include "constants.hpp"

#include <CoreMinimal.h>
#include "Amap_system.generated.h"


class Agame_mode;
class UMaterialInstanceDynamic;
class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
struct system_t;

USTRUCT()
struct Fsystem_graphical_properties
{
    GENERATED_BODY()

    UPROPERTY()
    int32 star_class = 0;

    UPROPERTY()
    int32 material_index = 0;

    UPROPERTY()
    int32 lens_flare_index = 0;
};

UCLASS()
class Amap_system : public Amap_pawn_base
{
    GENERATED_BODY()

public:
    Amap_system();

    void BeginPlay() override;
    void Tick(float delta) override;

    void select(bool b) override;
    void hover(bool b) override;
    map_pawn_kind kind() const override { return map_pawn_kind::system; }

    int system_id() { return system_id_; }

    void generate_graphical_properties(system_t const & system);
    void main_material(UMaterialInstanceDynamic * mid);
    void selection_materials(
        UMaterialInstanceDynamic * selected,
        UMaterialInstanceDynamic * hovered);

protected:
    void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty> & props) const override;

private:
    void system_id(int id) { system_id_ = id; }

    int system_id_ = system_none;

    UPROPERTY(ReplicatedUsing = OnRep_graphical_properties)
    Fsystem_graphical_properties graphical_properties_;
    UFUNCTION()
    void OnRep_graphical_properties();

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> main_mid_;
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> selected_mid_;
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> hovered_mid_;

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
    TObjectPtr<USphereComponent> sphere_;

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
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTextRenderComponent> system_name_;

    friend Agame_mode;
};
