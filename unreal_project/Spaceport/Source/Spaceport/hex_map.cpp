// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "hex.h"
#include "utility.hpp"
#include "hex_operations.hpp"

#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

#include <algorithm>
#include <memory>
#include <string>


namespace {

    std::string load_text_file (const std::string & name)
    {
        std::string retval;
        auto & platform_file = FPlatformFileManager::Get().GetPlatformFile();
        FString map_filename = FPaths::GameDir() + name.c_str();
        std::unique_ptr<IFileHandle> file_handle(platform_file.OpenRead(*map_filename));
        if (file_handle) {
            auto const size = file_handle->Size();
            retval = std::string(size, '\0');
            file_handle->Read((uint8 *)&retval[0], size);
        } else {
            auto const error = "Unable to load text file resource '" + map_filename + "'.";
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, *error);
        }
        return retval;
    }

    // Move to editable CSV.
    std::map<std::string, FLinearColor> const & primary_colors ()
    {
        static std::map<std::string, FLinearColor> retval;
        if (retval.empty()) {
            retval["FED"] = FLinearColor(57 / 255.0f, 142 / 255.0f, 230 / 255.0f);
            retval["GOR"] = FLinearColor(1, 1, 1);
            retval["HYD"] = FLinearColor(59 / 255.0f, 137 / 255.0f, 92 / 255.0f);
            retval["ISC"] = FLinearColor(89 / 255.0f, 89 / 255.0f, 89 / 255.0f);
            retval["KLI"] = FLinearColor(0, 0, 0);
            retval["KZI"] = FLinearColor(1, 1, 1);
            retval["LDR"] = FLinearColor(89 / 255.0f, 89 / 255.0f, 89 / 255.0f);
            retval["LYR"] = FLinearColor(241 / 255.0f, 241 / 255.0f, 76 / 255.0f);
            retval["NZ"] =  FLinearColor(136 / 255.0f, 136 / 255.0f, 136 / 255.0f);
            retval["ORI"] = FLinearColor(89 / 255.0f, 89 / 255.0f, 89 / 255.0f);
            retval["ROM"] = FLinearColor(221 / 255.0f, 0 ,0);
            retval["THO"] = FLinearColor(221 / 255.0f, 0 ,0);
            retval["WYN"] = FLinearColor(89 / 255.0f, 89 / 255.0f, 89 / 255.0f);
        }
        return retval;
    }

    // Move to editable CSV.
    std::map<std::string, FLinearColor> const & secondary_colors ()
    {
        static std::map<std::string, FLinearColor> retval;
        if (retval.empty()) {
            retval["FED"] = FLinearColor(0, 0, 0);
            retval["GOR"] = FLinearColor(221 / 255.0f, 0 ,0);
            retval["HYD"] = FLinearColor(1, 1, 1);
            retval["ISC"] = FLinearColor(0, 0, 0);
            retval["KLI"] = FLinearColor(1, 1, 1);
            retval["KZI"] = FLinearColor(0, 0, 0);
            retval["LDR"] = FLinearColor(0, 0, 0);
            retval["LYR"] = FLinearColor(58 / 255.0f, 138 / 255.0f, 45 / 255.0f);
            retval["NZ"] =  FLinearColor(0, 0, 0);
            retval["ORI"] = FLinearColor(0, 0, 0);
            retval["ROM"] = FLinearColor(0, 0, 0);
            retval["THO"] = FLinearColor(1, 1, 1);
            retval["WYN"] = FLinearColor(0, 0, 0);
        }
        return retval;
    }

    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60.0f));
}


Ahex_map::Ahex_map () :
    hex_mesh_ (nullptr),
    hex_mat_ (nullptr),
    hex_border_mat_ (nullptr),
    hex_border_mesh_ (nullptr),
    player_controller_ (nullptr),
    hexes_spawned_ (false)
{
    PrimaryActorTick.bCanEverTick = true;

    // TODO: Move the start-data loading code to a more appropriate location.
    start_data_.init_nations(load_text_file("nations.json"));
    start_data_.init_unit_defs(load_text_file("units.json"));
    auto load = [](const std::string & filename) { return load_text_file(filename); };
    start_data_.init_scenario(load_text_file("scenarios/the_wind.json"), load, load);

    instanced_hexes_.resize(primary_colors().size());
    instanced_borders_.resize(primary_colors().size()); // TODO
    for (auto const & pair : primary_colors()) {
        auto const nation_id = start_data_.nation(pair.first).nation_id;

        {
            auto name = pair.first + "_hexes";
            auto instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_hexes_[nation_id] = instanced;
        }

        {
            auto name = pair.first + "_borders"; // TODO
            auto instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_borders_[nation_id] = instanced;
        }
    }
}

void Ahex_map::BeginPlay ()
{
    Super::BeginPlay();
    call_real_soon(spawn_timer_, this, &Ahex_map::spawn_hexes);
}

void Ahex_map::Tick (float delta_seconds)
{
    // TODO: Remove.
    hex_coord_t const hc = hex_under_cursor();
    if (hc != invalid_hex_coord) {
        GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::White, start_data_.hex_string(hc));
    }
    else GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::White, "-1,-1");
}

hex_coord_t Ahex_map::hex_under_cursor () const
{
    hex_coord_t retval = invalid_hex_coord;

    if (!player_controller_ || !hexes_spawned_)
        return retval;

    FVector origin, direction;
    player_controller_->DeprojectMousePositionToWorld(origin, direction);

    FVector const terminus = origin + direction * 100000.0f;

    float t;
    FVector intersection;
    if (!UKismetMathLibrary::LinePlaneIntersection(
        origin,
        terminus,
        FPlane(FVector(0, 0, 0), FVector(0, 0, 1)),
        t,
        intersection)) {
        return retval;
    }

    intersection /= 100.0f;

    intersection.X += 1.0;
    intersection.Y += sin_60;

    // "Loose" because the point may fall above the upper-left edge or
    // below the lower-left edge.
    int const loose_column = FMath::FloorToInt(intersection.X / 1.5);

    if (loose_column % 2 == 1)
        intersection.Y += sin_60;

    int const loose_row = FMath::FloorToInt(intersection.Y / (2.0f * sin_60));

    hex_coord_t hc = {loose_column, start_data_.map().height - 1 - loose_row};
    if (FMath::Fmod(intersection.X, 1.5) < 0.5) {
        FVector2D left_corner(0, sin_60);
        FVector2D left_corner_to_point =
            FVector2D(intersection.X - loose_column * 1.5, intersection.Y - loose_row * 2 * sin_60) -
            left_corner;
        left_corner_to_point.Normalize();
        float const angle = FMath::Acos(FVector2D::DotProduct(FVector2D(1, 0), left_corner_to_point));
        if (FMath::DegreesToRadians(60.0f) < angle) {
            hc = 0 < left_corner_to_point.Y ? hex_above_left(hc) : hex_below_left(hc);
        }
    }

    retval = hc;

    if (!on_map(hc, start_data_.map()))
        retval = invalid_hex_coord;

    return retval;
}

void Ahex_map::hex_under_cursor (int & x, int & y) const
{
    hex_coord_t hc = hex_under_cursor();
    x = hc.x;
    y = hc.y;
}

void Ahex_map::spawn_hexes ()
{
    if (!hex_mesh_ || !hex_border_mesh_ || !hex_mat_ || !hex_border_mat_) {
        call_real_soon(spawn_timer_, this, &Ahex_map::spawn_hexes);
        return;
    }

    for (auto instanced : instanced_hexes_) {
        instanced->SetStaticMesh(hex_mesh_);
    }

    for (auto instanced : instanced_borders_) { // TODO
        instanced->SetStaticMesh(hex_border_mesh_);
    }

    // TODO: Probably this mapping should exist somewhere else as well.
    nation_id_primary_colors_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_primary_colors_[nation_id] = pair.second;
        auto instanced = instanced_hexes_[nation_id];
        UMaterialInstanceDynamic * material =
            instanced->CreateAndSetMaterialInstanceDynamicFromMaterial(0, hex_mat_);
        material->SetVectorParameterValue("color", pair.second);
    }
    nation_id_secondary_colors_.resize(secondary_colors().size());
    for (auto const & pair : secondary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_secondary_colors_[nation_id] = pair.second;
        auto instanced = instanced_borders_[nation_id]; // TODO: These are really the thick borders....
        UMaterialInstanceDynamic * material =
            instanced->CreateAndSetMaterialInstanceDynamicFromMaterial(0, hex_border_mat_);
        material->SetVectorParameterValue("color", pair.second);
    }

    auto const & map = start_data_.map();
    for (int x = 0; x < map.width; ++x) {
        for (int y = 0; y < map.height; ++y) {
            hex_coord_t const hc = {x, y};
            spawn_hex(hc);
        }
    }

    hexes_spawned_ = true;
}

void Ahex_map::spawn_hex (hex_coord_t hc)
{
    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60));

    auto const & map = start_data_.map();
    hex_t const & map_hex = map.hexes[hex_index(hc, map.width)];

    int const x = map_hex.coord.x;
    int const y = map_hex.coord.y;

    FRotator rotation = {0.0f, 0.0f, 0.0f};

    FVector location;
    location.X = x * 1.5 * meters;
    location.Y = (map.height - 1 - y) * 2 * sin_60 * meters;
    if ((x + 1000) % 2 == 1)
        location.Y -= sin_60 * meters;
    location.Z = 0.5f * meters;

    instanced_hexes_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));

    rotation.Roll = 180.0f;
    location.Z = 0 * meters;

    instanced_borders_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
    rotation.Yaw += 60.0f;
    instanced_borders_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
    rotation.Yaw += 60.0f;
    instanced_borders_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
    rotation.Yaw += 60.0f;
    instanced_borders_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
    rotation.Yaw += 60.0f;
    instanced_borders_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
    rotation.Yaw += 60.0f;
    instanced_borders_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
}
