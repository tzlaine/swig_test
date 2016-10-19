// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "root_widget.h"


Uroot_widget::Uroot_widget (FObjectInitializer const & init) :
    UUserWidget (init),
    panel_ (nullptr)
{}

UCanvasPanel * Uroot_widget::panel ()
{
    if (!panel_) {
        if (!WidgetTree)
            return nullptr;

        panel_ = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
        WidgetTree->RootWidget = panel_;

        SetAnchorsInViewport(FAnchors(0, 0, 1, 1));

        // TODO: Necessary? SetDesiredSizeInViewport(FVector2D(512/*TODO*/, 512/*TODO*/));
    }

    return panel_;
}

void Uroot_widget::NativeTick (FGeometry const & geometry, float dt)
{
    Super::NativeTick(geometry, dt);

    FVector2D size = geometry.GetLocalSize() * geometry.Scale;

    float const epsilon = -0.5f;
    if (!size.Equals(prev_size_, epsilon) && resize_callback_)
        resize_callback_(size.X, size.Y);

    prev_size_ = size;
}

void Uroot_widget::set_resize_callback (resize_callback_t const & callback)
{ resize_callback_ = callback; }
