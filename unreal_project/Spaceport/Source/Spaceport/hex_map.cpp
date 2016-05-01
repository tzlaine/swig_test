// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
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
    std::map<std::string, FColor> const & primary_colors ()
    {
        static std::map<std::string, FColor> retval;
        if (retval.empty()) {
            retval["FED"] = FColor(57, 142, 230);
            retval["GOR"] = FColor(255, 255, 255);
            retval["HYD"] = FColor(59, 137, 92);
            retval["ISC"] = FColor(89, 89, 89);
            retval["KLI"] = FColor(0, 0, 0);
            retval["KZI"] = FColor(255, 255, 255);
            retval["LDR"] = FColor(89, 89, 89);
            retval["LYR"] = FColor(241, 241, 76);
            retval["NZ"] =  FColor(136, 136, 136);
            retval["ORI"] = FColor(89, 89, 89);
            retval["ROM"] = FColor(221, 0 ,0);
            retval["THO"] = FColor(221, 0 ,0);
            retval["WYN"] = FColor(89, 89, 89);
        }
        return retval;
    }

    // Move to editable CSV.
    std::map<std::string, FColor> const & secondary_colors ()
    {
        static std::map<std::string, FColor> retval;
        if (retval.empty()) {
            retval["FED"] = FColor(0, 0, 0);
            retval["GOR"] = FColor(221, 0 ,0);
            retval["HYD"] = FColor(255, 255, 255);
            retval["ISC"] = FColor(0, 0, 0);
            retval["KLI"] = FColor(255, 255, 255);
            retval["KZI"] = FColor(0, 0, 0);
            retval["LDR"] = FColor(0, 0, 0);
            retval["LYR"] = FColor(58, 138, 45);
            retval["NZ"] =  FColor(0, 0, 0);
            retval["ORI"] = FColor(0, 0, 0);
            retval["ROM"] = FColor(0, 0, 0);
            retval["THO"] = FColor(255, 255, 255);
            retval["WYN"] = FColor(0, 0, 0);
        }
        return retval;
    }

    // TODO: Put this in a utility header or something.
    int owner (hex_coord_t hc, start_data::start_data_t const & start_data, game_data_t const & game_data)
    {
        int const province_id = start_data.hex_province(hc);
        if (province_id == -1)
            return start_data.neutral_zone_id();
        return game_data.province(province_id)->owner;
    }

    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60.0f));
    float const national_border_thickness = 1.0f;
    float const province_border_thickness = 0.65f;

    FVector hex_location (hex_coord_t hc, map_t const & map)
    {
        FVector location;
        location.X = hc.x * 1.5 * meters;
        location.Y = (map.height - 1 - hc.y) * 2 * sin_60 * meters;
        if ((hc.x + 1000) % 2 == 1)
            location.Y -= sin_60 * meters;
        return location;
    }

}


Ahex_map::Ahex_map () :
    hover_indicator_mesh_ (nullptr),
    interior_hex_mesh_ (nullptr),
    edge_hex_mesh_ (nullptr),
    planet_ (nullptr),
    star_5_ (nullptr),
    star_6_ (nullptr),
    star_8_ (nullptr),
    solid_color_mat_ (nullptr),
    hex_border_mat_ (nullptr),
    hex_border_mesh_ (nullptr),
    thin_hex_border_mat_ (nullptr),
    player_controller_ (nullptr),
    hover_indicator_ (nullptr),
    hexes_spawned_ (false)
{
    PrimaryActorTick.bCanEverTick = true;

    // TODO: Move the start-data loading code to a more appropriate location.
    start_data_.init_nations(load_text_file("nations.json"));
    start_data_.init_unit_defs(load_text_file("units.json"));
    auto load = [](const std::string & filename) { return load_text_file(filename); };
    start_data_.init_scenario(load_text_file("scenarios/the_wind.json"), load, load);
    game_data_ = game_data_t(start_data_);

    hover_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>("hover_indicator");
    hover_indicator_->AttachTo(RootComponent);

    instanced_interior_hexes_.resize(primary_colors().size());
    instanced_edge_hexes_.resize(primary_colors().size());
    instanced_national_borders_.resize(primary_colors().size());
    instanced_province_borders_.resize(primary_colors().size());
    instanced_hex_borders_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        auto const nation_id = start_data_.nation(pair.first).nation_id;

        {
            auto name = pair.first + "_interior_hexes";
            auto & instanced = instanced_interior_hexes_by_color_[pair.second];
            if (!instanced)
                instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_interior_hexes_[nation_id] = instanced;
        }

        {
            auto name = pair.first + "_edge_hexes";
            auto & instanced = instanced_edge_hexes_by_color_[pair.second];
            if (!instanced)
                instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_edge_hexes_[nation_id] = instanced;
        }
    }

    for (auto const & pair : secondary_colors()) {
        auto const nation_id = start_data_.nation(pair.first).nation_id;

        {
            auto name = pair.first + "_national_borders";
            auto & instanced = instanced_national_borders_by_color_[pair.second];
            if (!instanced)
                instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_national_borders_[nation_id] = instanced;
        }

        {
            auto name = pair.first + "_province_borders";
            auto & instanced = instanced_province_borders_by_color_[pair.second];
            if (!instanced)
                instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_province_borders_[nation_id] = instanced;
        }

        {
            auto name = pair.first + "_hex_borders";
            auto & instanced = instanced_hex_borders_by_color_[pair.second];
            if (!instanced)
                instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
            instanced->AttachTo(RootComponent);
            instanced_hex_borders_[nation_id] = instanced;
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
    hex_coord_t const hc = hex_under_cursor();
    if (hc != invalid_hex_coord) {
        auto location = hex_location(hc, game_data_.map());
        location.Z = -0.05 * meters;
        hover_indicator_->SetWorldTransform(FTransform(FRotator(0, 0, 0), location));

        auto const owner_id = owner(hc, start_data_, game_data_);
        auto const color = nation_id_secondary_colors_[owner_id];
        use_solid_color(hover_indicator_, color);

        hover_indicator_->SetVisibility(true);
    } else {
        hover_indicator_->SetVisibility(false);
    }
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

    hex_coord_t hc = {loose_column, game_data_.map().height - 1 - loose_row};
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

    if (!on_map(hc, game_data_.map()))
        retval = invalid_hex_coord;

    return retval;
}

void Ahex_map::hex_under_cursor (int & x, int & y) const
{
    hex_coord_t hc = hex_under_cursor();
    x = hc.x;
    y = hc.y;
}

void Ahex_map::initialize_border_instanced_mesh (national_instances_t & instanced_meshes, int nation_id, float thickness, UMaterial * mat)
{
    auto instanced = instanced_meshes[nation_id];
    UMaterialInstanceDynamic * material =
        instanced->CreateAndSetMaterialInstanceDynamicFromMaterial(0, mat);
    auto const color = nation_id_secondary_colors_[nation_id];
    material->SetVectorParameterValue("color", color.ReinterpretAsLinear());
    material->SetScalarParameterValue("thickness", thickness);
}

void Ahex_map::use_solid_color(UStaticMeshComponent * instanced, FColor color)
{
    auto & material = solid_color_materials_[color];
    if (material) {
        instanced->SetMaterial(0, material);
    } else {
        material = instanced->CreateAndSetMaterialInstanceDynamicFromMaterial(0, solid_color_mat_);
        auto const linear_color = color.ReinterpretAsLinear();
        material->SetVectorParameterValue("color", linear_color);
    }
}

void Ahex_map::spawn_hexes ()
{
    if (!hover_indicator_mesh_ ||
        !interior_hex_mesh_ ||
        !edge_hex_mesh_ ||
        !planet_ ||
        !star_5_ ||
        !star_6_ ||
        !star_8_ ||
        !solid_color_mat_ ||
        !hex_border_mesh_ ||
        !hex_border_mat_ ||
        !thin_hex_border_mat_) {
        call_real_soon(spawn_timer_, this, &Ahex_map::spawn_hexes);
        return;
    }

    hover_indicator_->SetStaticMesh(hover_indicator_mesh_);
    hover_indicator_->SetVisibility(false);

    for (auto instanced : instanced_interior_hexes_) {
        instanced->SetStaticMesh(interior_hex_mesh_);
    }
    for (auto instanced : instanced_edge_hexes_) {
        instanced->SetStaticMesh(edge_hex_mesh_);
    }

    for (auto instanced : instanced_national_borders_) {
        instanced->SetStaticMesh(hex_border_mesh_);
    }
    for (auto instanced : instanced_province_borders_) {
        instanced->SetStaticMesh(hex_border_mesh_);
    }
    for (auto instanced : instanced_hex_borders_) {
        instanced->SetStaticMesh(hex_border_mesh_);
    }

    // TODO: Probably this mapping should exist somewhere else as well.
    nation_id_primary_colors_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_primary_colors_[nation_id] = pair.second;
        use_solid_color(instanced_interior_hexes_[nation_id], pair.second);
        use_solid_color(instanced_edge_hexes_[nation_id], pair.second);
    }

    nation_id_secondary_colors_.resize(secondary_colors().size());
    for (auto const & pair : secondary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_secondary_colors_[nation_id] = pair.second;
        initialize_border_instanced_mesh(instanced_national_borders_, nation_id, national_border_thickness, hex_border_mat_);
        initialize_border_instanced_mesh(instanced_province_borders_, nation_id, province_border_thickness, hex_border_mat_);
        initialize_border_instanced_mesh(instanced_hex_borders_, nation_id, 1.0f, thin_hex_border_mat_);
    }

    auto const & map = game_data_.map();
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
    auto const & map = game_data_.map();
    auto const & map_hex = game_data_.hex(hc);
    auto const * province = game_data_.province(map_hex.province_id);
    auto const owner_id = owner(hc, start_data_, game_data_);

    int const x = map_hex.coord.x;
    int const y = map_hex.coord.y;

    FRotator rotation = {0.0f, 0.0f, 0.0f};

    FVector location = hex_location(map_hex.coord, map);
    location.Z = 0.5f * meters;

    if (x == 0 || y == 0 || x == map.width - 1 || y == map.height - 1)
        instanced_edge_hexes_[owner_id]->AddInstanceWorldSpace(FTransform(rotation, location));
    else
        instanced_interior_hexes_[owner_id]->AddInstanceWorldSpace(FTransform(rotation, location));

    rotation.Roll = 180.0f;
    location.Z = 0 * meters;

    for (auto d : all_hex_directions) {
        auto other_hc = adjacent_hex_coord(hc, d);
        if (!on_map(other_hc, map)) {
            instanced_national_borders_[owner_id]->AddInstanceWorldSpace(FTransform(rotation, location));
        } else {
            auto const & other_hex = game_data_.hex(other_hc);
            auto const * other_province = game_data_.province(other_hex.province_id);
            auto const other_owner_id = owner(other_hc, start_data_, game_data_);
            if (other_owner_id != owner_id)
                instanced_national_borders_[owner_id]->AddInstanceWorldSpace(FTransform(rotation, location));
            else if (other_hex.province_id != map_hex.province_id)
                instanced_province_borders_[owner_id]->AddInstanceWorldSpace(FTransform(rotation, location));
            else
                instanced_hex_borders_[owner_id]->AddInstanceWorldSpace(FTransform(rotation, location));
        }
        rotation.Yaw += 60.0f;
    }
}
