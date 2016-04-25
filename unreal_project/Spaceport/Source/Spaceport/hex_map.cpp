// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "hex.h"
#include "utility.hpp"
#include "model.hpp"
#include "hex_operations.hpp"

#include <algorithm>
#include <memory>
#include <string>


namespace {

    std::string load_text_file(const std::string & name)
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
    std::map<std::string, FLinearColor> const & primary_colors()
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
            retval["LYR"] = FLinearColor(241 / 255.0f, 241 / 255.0f, 241 / 255.0f);
            retval["NZ"] =  FLinearColor(136 / 255.0f, 136 / 255.0f, 136 / 255.0f);
            retval["ORI"] = FLinearColor(89 / 255.0f, 89 / 255.0f, 89 / 255.0f);
            retval["ROM"] = FLinearColor(221 / 255.0f, 0 ,0);
            retval["THO"] = FLinearColor(221 / 255.0f, 0 ,0);
            retval["WYN"] = FLinearColor(89 / 255.0f, 89 / 255.0f, 89 / 255.0f);
        }
        return retval;
    }

    // Move to editable CSV.
    std::map<std::string, FLinearColor> const & secondary_colors()
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
}

void Ahex_map::BeginPlay ()
{
    Super::BeginPlay();
    call_real_soon(spawn_timer, this, &Ahex_map::spawn_hexes);
}

void Ahex_map::spawn_hexes()
{
    UWorld * const world = GetWorld();
    if (!hex || !world) {
        call_real_soon(spawn_timer, this, &Ahex_map::spawn_hexes);
        return;
    }

    // TODO: Move the start-data loading code to a more appropriate location.
    start_data_t start_data;
    start_data.init_nations(load_text_file("nations.json"));
    start_data.init_unit_defs(load_text_file("units.json"));
    auto load = [](const std::string & filename) { return load_text_file(filename); };
    start_data.init_scenario(load_text_file("scenarios/the_wind.json"), load, load);

    // TODO: Probably this mapping should exist somewhere else as well.
    nation_id_primary_colors_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        if (pair.first == "NZ")
            nation_id_primary_colors_[0] = pair.second;
        else
            nation_id_primary_colors_[start_data.nation(pair.first).nation_id] = pair.second;
    }
    nation_id_secondary_colors_.resize(secondary_colors().size());
    for (auto const & pair : secondary_colors()) {
        if (pair.first == "NZ")
            nation_id_secondary_colors_[0] = pair.second;
        else
            nation_id_secondary_colors_[start_data.nation(pair.first).nation_id] = pair.second;
    }

    auto const & map = start_data.map();
    for (int x = 0; x < map.width; ++x) {
        for (int y = 0; y < map.height; ++y) {
            hex_coord_t const hc = {x, y};
            hex_t const & map_hex = map.hexes[hex_index(hc, map.width)];
            Ahex * new_hex = spawn_hex(map_hex, map.width, map.height, world);
        }
    }
}

Ahex * Ahex_map::spawn_hex (hex_t const & map_hex, int map_width, int map_height, UWorld * const world)
{
    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60));

    int const x = map_hex.coord.x;
    int const y = map_hex.coord.y;

    FVector location;
    location.X = x * 1.5 * meters;
    location.Y = (map_height - 1 - y) * 2 * sin_60 * meters;
    if ((x + 1000) % 2 == 1)
        location.Y -= sin_60 * meters;
    location.Z = 0 * meters;

    FRotator rotation = {0.0f, 0.0f, 0.0f};

    FActorSpawnParameters spawn_params;
    spawn_params.Owner = this;
    spawn_params.Instigator = Instigator;

    Ahex * retval = world->SpawnActor<Ahex>(hex, location, rotation, spawn_params);

    TArray<UStaticMeshComponent *> components;
    retval->GetComponents<UStaticMeshComponent>(components);
    assert(components.Num() == 1);
    UStaticMeshComponent * static_mesh_component = components[0];
    static_mesh_component->SetCastShadow(
        x == 0 || x == map_width - 1 || y == 0 || y == map_height - 1
    );

    UMaterialInstanceDynamic * material =
        static_mesh_component->CreateAndSetMaterialInstanceDynamicFromMaterial(
            0,
            static_mesh_component->GetMaterial(0u)
        );
    FLinearColor color = nation_id_primary_colors_[map_hex.owner];
    material->SetVectorParameterValue("color", color);

    retval->set_position(x, y);

    return retval;
}
