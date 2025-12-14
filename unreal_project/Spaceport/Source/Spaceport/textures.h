#pragma once

#include <CoreMinimal.h>
#include <SlateCore.h>
#include "textures.generated.h"


class UTexture;

UCLASS(BlueprintType, Blueprintable)
class Utextures_t : public UObject
{
    GENERATED_BODY()

public:
    // Map stars
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> wide_lens_flare_0_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> wide_lens_flare_1_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> wide_lens_flare_2_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> wide_lens_flare_3_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> wide_lens_flare_4_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> small_lens_flare_0_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> small_lens_flare_1_;

    // Planets
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_0_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_1_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_2_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_3_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_4_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_5_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_6_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_7_;
    UPROPERTY(EditAnywhere, Category = "Textures")
    TObjectPtr<UTexture> planet_texture_8_;

    int random_wide_lens_flare() const;
    int random_small_lens_flare() const;
    UTexture * wide_lens_flare(int i) const;
    UTexture * small_lens_flare(int i) const;
    UTexture * random_planet_texture() const;
};

Utextures_t const & textures();
