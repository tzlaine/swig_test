// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "hex.h"
#include "utility.hpp"
#include "model.hpp"
#include <memory>
#include <string>


namespace {

    std::string load_text_file(const std::string & name)
    {
        std::string retval;
        auto& platform_file = FPlatformFileManager::Get().GetPlatformFile();
        FString map_filename = FPaths::GameDir() + name.c_str();
        std::unique_ptr<IFileHandle> file_handle(platform_file.OpenRead(*map_filename));
        if (file_handle) {
            auto const size = file_handle->Size();
            retval = std::string(size, '\0');
            file_handle->Read((uint8*)&retval[0], size);
        } else {
            auto const error = "Unable to load text file resource '" + map_filename + "'.";
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, *error);
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
    UWorld* const world = GetWorld();
    if (!hex || !world) {
        call_real_soon(spawn_timer, this, &Ahex_map::spawn_hexes);
        return;
    }

    // TODO: Move the map loading code to a more apporpriate location.
    start_data_t start_data;
    start_data.init_nations(load_text_file("nations.json"));
    start_data.init_unit_defs(load_text_file("units.json"));
    auto load = [](const std::string & filename) { return load_text_file(filename); };
    start_data.init_scenario(load_text_file("scenarios/the_wind.json"), load, load);

    auto map = start_data.map();
    for (int x = 0; x < map.width; ++x) {
        for (int y = 0; y < map.height; ++y) {
            Ahex* new_hex = spawn_hex(x, y, map.width, map.height, world);
            // TODO: Assign values to the hex.
        }
    }
}

Ahex* Ahex_map::spawn_hex (int x, int y, int map_width, int map_height, UWorld* const world)
{
    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60));

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

    Ahex* retval = world->SpawnActor<Ahex>(hex, location, rotation, spawn_params);

    TArray<UStaticMeshComponent*> components;
    retval->GetComponents<UStaticMeshComponent>(components);
    assert(components.Num() == 1);
    UStaticMeshComponent* static_mesh_component = components[0];
    static_mesh_component->SetCastShadow(
        x == 0 || x == map_width - 1 || y == 0 || y == map_height - 1
    );
    retval->set_position(x, y);
    return retval;
}
