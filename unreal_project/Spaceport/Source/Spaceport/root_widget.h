// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/PanelWidget.h"
#include "Blueprint/UserWidget.h"
#include <boost/function.hpp>

#include "root_widget.generated.h"

template <typename T>
struct widget_and_slot_t
{
    T * widget_;
    UCanvasPanelSlot * slot_;
};

/** The root element in every HUD or dialog box. */
UCLASS()
class SPACEPORT_API Uroot_widget : public UUserWidget
{
    GENERATED_BODY()
public:
    Uroot_widget (FObjectInitializer const & init);

    /** Returns the canvas panel used to lay out this widget's children;
        creates it if it does not already exist.  may return nullptr if
        creation fails. */
    UCanvasPanel * panel ();

    template <typename T>
    widget_and_slot_t<T> new_child (UClass * class_ = nullptr)
    {
        if (!class_)
            class_ = T::StaticClass();
        auto child = WidgetTree->ConstructWidget<T>(class_);
        return widget_and_slot_t<T>{child, panel()->AddChildToCanvas(child)};
    }

    using resize_callback_t = boost::function<void (long, long)>;

    void set_resize_callback (resize_callback_t const & callback);

protected:
    virtual void NativeTick (FGeometry const & geometry, float dt) override;

private:
    UCanvasPanel * panel_;
    resize_callback_t resize_callback_;
    FVector2D prev_size_;
};
