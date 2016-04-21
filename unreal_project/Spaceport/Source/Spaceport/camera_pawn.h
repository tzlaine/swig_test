// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "camera_pawn.generated.h"

UCLASS()
class SPACEPORT_API Acamera_pawn : public APawn
{
	GENERATED_BODY()

public:
    Acamera_pawn ();

    virtual void BeginPlay () override;
	
    virtual void Tick (float DeltaSeconds) override;

    virtual void SetupPlayerInputComponent (class UInputComponent* InputComponent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "camera_pawn")
    float rotation_speed_;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USphereComponent* z_axis_gimbal_;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USphereComponent* x_axis_gimbal_;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* spring_arm_;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* camera_;

    APlayerController* player_controller_;

    enum class drag
    {
        none,
        rotation,
        map_drag
    };

    bool mouse_button_down_[3]; // l, r, m
    drag drag_;

    void check_for_drag ()
    {
#if 0
        {
            bool const lmb_down = player_controller_->IsInputKeyDown(EKeys::LeftMouseButton);
            if (lmb_down != mouse_button_down_[0]) {
                if (lmb_down)
                    camera_rotate_begin();
                else
                    camera_rotate_end();
                mouse_button_down_[0] = lmb_down;
            }
        }
#endif

        {
            bool const mmb_down = player_controller_->IsInputKeyDown(EKeys::MiddleMouseButton);
            if (mmb_down != mouse_button_down_[1]) {
                if (mmb_down)
                    camera_rotate_begin();
                else
                    camera_rotate_end();
                mouse_button_down_[1] = mmb_down;
            }
        }

        {
            bool const rmb_down = player_controller_->IsInputKeyDown(EKeys::RightMouseButton);
            if (rmb_down != mouse_button_down_[2]) {
                if (rmb_down)
                    map_drag_begin();
                else
                    map_drag_end();
                mouse_button_down_[2] = rmb_down;
            }
        }
    }

    void camera_rotate_begin ()
    {
//        GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::White, TEXT("camera_rotate_begin() (1)"));
        if (drag_ != drag::none)
            return;

        player_controller_->GetMousePosition(prev_mouse_drag_x_, prev_mouse_drag_y_);
        player_controller_->bShowMouseCursor = false;
        drag_ = drag::rotation;
    }

    void camera_rotate_end ()
    {
        if (drag_ != drag::rotation)
            return;

        player_controller_->bShowMouseCursor = true;
        drag_ = drag::none;
    }

    void map_drag_begin ()
    {
        if (drag_ != drag::none)
            return;

        player_controller_->GetMousePosition(prev_mouse_drag_x_, prev_mouse_drag_y_);
        player_controller_->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
        drag_ = drag::map_drag;
    }

    void map_drag_end ()
    {
        if (drag_ != drag::map_drag)
            return;

        player_controller_->CurrentMouseCursor = EMouseCursor::Default;
        drag_ = drag::none;
    }

    void drag_update ();

    float prev_mouse_drag_x_;
    float prev_mouse_drag_y_;
};
