#pragma once

class AActor;
class AStaticMeshActor;
class Amap_system;
struct planet_t;
struct star_t;
struct system_t;
struct Fsystem_graphical_properties;

Fsystem_graphical_properties generate_map_star(system_t const & system);
void configure_map_star(
    Amap_system & system_actor,
    Fsystem_graphical_properties const & props,
    bool capitol);

void configure_system_star(AActor * star_actor, star_t const & star);

void configure_rocky_oxidized_planet(
    AActor * planet_actor,
    planet_t const & planet,
    float population,
    float infrastructure);

void configure_rocky_reduced_or_carbon_rich_planet(
    AActor * planet_actor, planet_t const & planet);

void configure_high_temperature_planet(
    AActor * planet_actor, planet_t const & planet);

void configure_gas_giant_planet(AActor * planet_actor, planet_t const & planet);

void configure_ice_giant_planet(AActor * planet_actor, planet_t const & planet);
