#include "map_transition.hpp"

#include <GameFramework/Actor.h>


namespace {
    void set_hidden(
        bool b,
        AActor * system_star,
        TArray<AActor *> const & system_planets,
        TArray<AActor *> const & system_fleets)
    {
        system_star->SetActorHiddenInGame(b);
        for (auto * a : system_planets) {
            a->SetActorHiddenInGame(b);
        }
        for (auto * a : system_fleets) {
            a->SetActorHiddenInGame(b);
        }
    }
    float const system_star_far_z = 100000.0f;
}

void map_transition_state::to_system_map(
    FVector system_location,
    FVector camera_location,
    AActor * system_star,
    TArray<AActor *> & system_planets,
    TArray<AActor *> & system_fleets)
{
    check(mode_ == map_mode::galaxy_map);

    system_star_ = system_star;
    system_planets_ = &system_planets;
    system_fleets_ = &system_fleets;
    moving_camera_ = true;
    mode_ = map_mode::transition_to_system_map;

    transition_progress_ = 0.0f;
    initial_location_ = camera_location;
    final_location_ = system_location;
    final_location_.Z = just_inside_system_map;
    camera_location_ = camera_location;
    entered_system_view_location_ = final_location_;

    set_hidden(true, system_star_, *system_planets_, *system_fleets_);
}

void map_transition_state::to_galaxy_map(FVector camera_location)
{
    check(mode_ == map_mode::system_map);

    moving_camera_ = false;
    mode_ = map_mode::transition_to_galaxy_map;

    transition_progress_ = 0.0f;
    initial_location_ = system_star_->GetActorLocation();
    final_location_ = initial_location_;
    final_location_.Z = system_star_far_z;
    camera_location_ = camera_location;
}

void map_transition_state::tick(float delta)
{
    new_map_mode_ = {};
    new_camera_location_ = {};
    new_star_location_ = {};

    if (!::in_transition(mode_))
        return;

    float const close_enough = 0.001f;
    if (system_view_transition_time_s - close_enough < transition_progress_) {
        if (mode_ == map_mode::transition_to_system_map) {
            if (moving_camera_) {
                // Moving the camera was part 1; now zoom the star
                // upward into view.
                moving_camera_ = false;
                transition_progress_ = 0.0f;
                initial_location_ = system_star_->GetActorLocation();
                initial_location_.Z = system_star_far_z;
                system_star_->SetActorLocation(initial_location_);
                final_location_ = initial_location_;
                final_location_.Z = 0;
                set_hidden(
                    false, system_star_, *system_planets_, *system_fleets_);
            } else {
                new_map_mode_ = mode_ = map_mode::system_map;
            }
        } else {
            if (!moving_camera_) {
                // Moving the star downward out of view was part
                // 1; now move the camera.
                moving_camera_ = true;
                transition_progress_ = 0.0f;
                initial_location_ = camera_location_;
                final_location_ = entered_system_view_location_;
                final_location_.Z =
                    -(min_camera_dist_for(map_mode::galaxy_map) + 1);
                set_hidden(
                    true, system_star_, *system_planets_, *system_fleets_);
            } else {
                new_map_mode_ = mode_ = map_mode::galaxy_map;
            }
        }
        return;
    }

    float const smooth_alpha = FMath::SmoothStep(
        0.0f,
        1.0f,
        std::min(transition_progress_ / system_view_transition_time_s, 1.0f));

    if (moving_camera_) {
        new_camera_location_ =
            FMath::Lerp(initial_location_, final_location_, smooth_alpha);
        new_camera_location_->Z = FMath::Lerp(
            initial_location_.Z,
            final_location_.Z,
            FMath::Pow(smooth_alpha, 3));
        transition_progress_ += delta;
    } else {
        new_star_location_ =
            FMath::Lerp(initial_location_, final_location_, smooth_alpha);
        transition_progress_ += delta;
    }
}
