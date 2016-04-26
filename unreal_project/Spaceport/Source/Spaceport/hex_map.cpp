// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "hex.h"
#include "utility.hpp"
#include "hex_operations.hpp"

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

}


Ahex_map::Ahex_map ()
{
    PrimaryActorTick.bCanEverTick = false;

    static_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("static_"));
    static_->SetVisibility(false);
    static_->AttachTo(RootComponent);

    // TODO: Move the start-data loading code to a more appropriate location.
    start_data_.init_nations(load_text_file("nations.json"));
    start_data_.init_unit_defs(load_text_file("units.json"));
    auto load = [](const std::string & filename) { return load_text_file(filename); };
    start_data_.init_scenario(load_text_file("scenarios/the_wind.json"), load, load);

    instanced_hexes_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        auto instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(pair.first.c_str());
        instanced->AttachTo(RootComponent);
        instanced_hexes_[start_data_.nation(pair.first).nation_id] = instanced;
    }
}

void Ahex_map::BeginPlay ()
{
    Super::BeginPlay();
    call_real_soon(spawn_timer_, this, &Ahex_map::spawn_hexes);
}

void Ahex_map::spawn_hexes()
{
    UWorld * const world = GetWorld();
    if (!world || !static_->StaticMesh) {
        call_real_soon(spawn_timer_, this, &Ahex_map::spawn_hexes);
        return;
    }

    for (auto instanced : instanced_hexes_) {
        instanced->SetStaticMesh(static_->StaticMesh);
    }

    // TODO: Probably this mapping should exist somewhere else as well.
    nation_id_primary_colors_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_primary_colors_[nation_id] = pair.second;
        auto instanced = instanced_hexes_[nation_id];
        UMaterialInstanceDynamic * material =
            instanced->CreateAndSetMaterialInstanceDynamicFromMaterial(
                0,
                instanced->GetMaterial(0u)
            );
        material->SetVectorParameterValue("color", pair.second);
    }
    nation_id_secondary_colors_.resize(secondary_colors().size());
    for (auto const & pair : secondary_colors()) {
        nation_id_secondary_colors_[start_data_.nation(pair.first).nation_id] = pair.second;
    }

    auto const & map = start_data_.map();
    for (int x = 0; x < map.width; ++x) {
        for (int y = 0; y < map.height; ++y) {
            hex_coord_t const hc = {x, y};
            hex_t const & map_hex = map.hexes[hex_index(hc, map.width)];
            spawn_hex(map_hex, map.width, map.height, world);
        }
    }
}

void Ahex_map::spawn_hex (hex_t const & map_hex, int map_width, int map_height, UWorld * const world)
{
    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60));

    int const x = map_hex.coord.x;
    int const y = map_hex.coord.y;

    FRotator const rotation = {0.0f, 0.0f, 0.0f};

    FVector location;
    location.X = x * 1.5 * meters;
    location.Y = (map_height - 1 - y) * 2 * sin_60 * meters;
    if ((x + 1000) % 2 == 1)
        location.Y -= sin_60 * meters;
    location.Z = 0 * meters;

    instanced_hexes_[map_hex.owner]->AddInstanceWorldSpace(FTransform(rotation, location));
}
