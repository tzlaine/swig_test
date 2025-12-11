#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>
#include <Engine/Font.h>
#include "materials.generated.h"


class UMaterialInterface;

UCLASS(BlueprintType, Blueprintable)
class Umaterials_t : public UObject
{
    GENERATED_BODY()

public:
    // UI
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> rotator_pip_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> slider_fill_;

    // Map stars
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> blue_map_star_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> blue_white_map_star_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> white_map_star_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> yellow_map_star_0_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> yellow_map_star_1_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> red_map_star_0_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> red_map_star_1_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> red_map_star_2_;
    UPROPERTY(EditAnywhere, Category = "Materials")
    TObjectPtr<UMaterialInterface> system_selected_;
};

Umaterials_t const & materials();
