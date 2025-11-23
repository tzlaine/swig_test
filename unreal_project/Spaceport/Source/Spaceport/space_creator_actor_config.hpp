#pragma once

class AActor;
class AStaticMeshActor;
struct system_t;
struct star_t;
struct planet_t;
struct loaded_textures;
struct loaded_material_interfaces;

void configure_map_star(
    AStaticMeshActor * star_actor,
    system_t const & system,
    star_t const & star,
    loaded_textures & textures,
    loaded_material_interfaces & material_interfaces);

void configure_system_star(AActor * star_actor, star_t const & star);

void configure_rocky_oxidized_planet(
    AActor * planet_actor, planet_t const & planet, loaded_textures & textures);

void configure_rocky_reduced_or_carbon_rich_planet(
    AActor * planet_actor, planet_t const & planet, loaded_textures & textures);

void configure_high_temperature_planet(
    AActor * planet_actor, planet_t const & planet, loaded_textures & textures);

void configure_gas_giant_planet(
    AActor * planet_actor, planet_t const & planet, loaded_textures & textures);

void configure_ice_giant_planet(
    AActor * planet_actor, planet_t const & planet, loaded_textures & textures);
