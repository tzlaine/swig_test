#include "Arender_target.h"

#include <Components/SceneCaptureComponent2D.h>
#include <Kismet/KismetRenderingLibrary.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>


Arender_target::Arender_target()
{
    root_ = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    capture_component_ = CreateDefaultSubobject<USceneCaptureComponent2D>(
        TEXT("capture_component"));

    RootComponent = root_;

    capture_component_->SetupAttachment(root_);
    capture_component_->bCaptureEveryFrame = true;
    capture_component_->PrimitiveRenderMode =
        ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    capture_component_->ProjectionType = ECameraProjectionMode::Orthographic;
}

void Arender_target::BeginPlay()
{
    int const image_size = 512;
    render_target_ = UKismetRenderingLibrary::CreateRenderTarget2D(
        this,
        image_size,
        image_size,
        ETextureRenderTargetFormat::RTF_RGBA16f,
        FLinearColor::Transparent);
    render_target_->UpdateResource();
    capture_component_->TextureTarget = render_target_;

    if (!material_)
        return;

    material_instance_ = UMaterialInstanceDynamic::Create(material_, this);
    material_instance_->SetTextureParameterValue(
        TEXT("texture"), render_target_);

    brush_.SetResourceObject(material_instance_);
}

void Arender_target::render_actor(AActor * a, float ortho_width)
{
    capture_component_->ShowOnlyActors.Empty();
    capture_component_->ShowOnlyActors.Add(a);
    capture_component_->OrthoWidth = ortho_width;
}
