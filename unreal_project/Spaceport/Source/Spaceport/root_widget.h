// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Components/PanelWidget.h"
#include "Blueprint/UserWidget.h"

#include "root_widget.generated.h"

/** The root element in every HUD or dialog box. */
UCLASS()
class SPACEPORT_API Uroot_widget : public UUserWidget
{
    GENERATED_BODY()
public:
    Uroot_widget(FObjectInitializer const & init);

    /** Returns the canvas panel used to lay out this widget's children;
        creates it if it does not already exist.  may return nullptr if
        creation fails. */
    UCanvasPanel * panel();

private:
    UCanvasPanel * panel_;
};
