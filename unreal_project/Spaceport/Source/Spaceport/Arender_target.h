#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <Styling/SlateBrush.h>
#include "Arender_target.generated.h"


class UMaterialInstanceDynamic;
class UMaterialInterface;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

UCLASS()
class Arender_target : public AActor
{
    GENERATED_BODY()

public:
    Arender_target();

    void BeginPlay() override;

    void render_actor(AActor * a);

    FSlateBrush const & brush() const { return brush_; }

private:
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> root_;

    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneCaptureComponent2D> capture_component_;

    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UTextureRenderTarget2D> render_target_;

    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMaterialInterface> material_;
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMaterialInstanceDynamic> material_instance_;

    FSlateBrush brush_;
};
