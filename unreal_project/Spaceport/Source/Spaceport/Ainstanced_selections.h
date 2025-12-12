#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "instanced_selections.generated.h"


UCLASS()
class SPACEPORT_API Ainstanced_selections : public AActor
{
    GENERATED_BODY()

public:
    Ainstanced_selections();

    virtual void BeginPlay() override;

    void dehover_all();
    void deselect_all();

    void hover(Amap_fleet * f);
    void dehover(Amap_system * f);
    void select(Amap_fleet * f);
    void deselect(Amap_system * f);

private:
    std::vector<int> curr_system_selections_;
    std::vector<int> curr_system_hovers_;

    // TODO
    std::vector<int> curr_fleet_selections_;
    std::vector<int> curr_fleet_hovers_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInstancedStaticMeshComponent> selected_systems_;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInstancedStaticMeshComponent> hovered_systems_;
};
