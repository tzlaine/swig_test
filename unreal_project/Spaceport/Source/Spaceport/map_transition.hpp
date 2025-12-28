#pragma once

#include "constants.hpp"
#include "map_transition_fwd.hpp"

#include <optional>
#include <string>

#include <CoreMinimal.h>


class AActor;

inline bool in_transition(map_mode mode)
{
    return (int)map_mode::transition_to_galaxy_map <= (int)mode;
}

inline std::string to_string(map_mode mode)
{
    switch (mode) {
    case map_mode::galaxy_map: return "galaxy_map";
    case map_mode::system_map: return "system_map";
    case map_mode::transition_to_galaxy_map: return "transition_to_galaxy_map";
    case map_mode::transition_to_system_map: return "transition_to_system_map";
    default: return "UNKNOWN map_mode";
    }
}

inline float min_camera_dist_for(map_mode mode)
{
    float retval = min_camera_dist;
    if (mode == map_mode::system_map)
        retval += map_actors_vertical_offset;
    return retval;
}

inline float max_camera_dist_for(map_mode mode)
{
    float retval = max_camera_dist;
    if (mode == map_mode::system_map)
        retval = min_camera_dist_for(map_mode::galaxy_map) + 1000;
    return retval;
}

struct map_transition_state
{
    bool in_transition() const { return ::in_transition(mode_); }
    std::optional<map_mode> new_map_mode() const { return new_map_mode_; }
    std::optional<FVector> new_camera_location() const
    {
        return new_camera_location_;
    }
    std::optional<FVector> new_star_location() const
    {
        return new_star_location_;
    }

    void to_system_map(
        FVector system_location,
        FVector camera_location,
        AActor * system_star,
        TArray<AActor *> & system_planets,
        TArray<AActor *> & system_fleets);
    void to_galaxy_map(FVector camera_location);
    void tick(float delta);

private:
    map_mode mode_ = map_mode::galaxy_map;
    AActor * system_star_ = nullptr;
    TArray<AActor *> * system_planets_ = nullptr;
    TArray<AActor *> * system_fleets_ = nullptr;
    bool moving_camera_ = false;
    float transition_progress_ = 0.0f;
    FVector initial_location_{};
    FVector final_location_{};
    FVector camera_location_{};
    FVector entered_system_view_location_{};
    std::optional<map_mode> new_map_mode_;
    std::optional<FVector> new_camera_location_;
    std::optional<FVector> new_star_location_;
};
