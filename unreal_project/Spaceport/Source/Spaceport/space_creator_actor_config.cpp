#include "space_creator_actor_config.hpp"

#include "Aplayer_controller_base.h"
#include "Amap_system.h"
#include "constants.hpp"
#include "materials.h"
#include "game_data.hpp"
#include "rng.hpp"
#include "textures.h"
#include "utility.hpp"

#include <boost/type_index.hpp>

#include <CoreMinimal.h>
#include <EngineUtils.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/DirectionalLight.h>
#include <Engine/StaticMesh.h>
#include <Engine/Texture.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Materials/MaterialInterface.h>


/* Map star notes (map stars use only the lense flare/ distant star graphics):

   The available star materials are: blue_map_star, blue_white_map_star,
   white_map_star, yellow_map_star_{1,2}, red_map_star_{1-3}

   Horizontal lense flare textures: T_LensFlare_{1-4,7}.  These are best used
   on the brightest stars.

   - Burst_Intensity: 0.5-20 for non-horizontal lense flares; 20.0-40.0 for
     horizontal lens flares (scaled partly by star brightness, adjusted by
     linear randomness)

   - Texture_Main_Flare: set to one of the T_LensFlare_{1-7}

   - Halo_Size: if horizontal lens flare is in use: 0.1-0.4, linear; 0.1-0.25,
     linear otherwise

   System star notes (uses the detailed star blueprint):
   - Atmospheric_Phenomena: Change to false only in the lowest graphics setting
   - Coronal_Rays_Count: Set to 128
   - Surface_Explosions_Count: Set to 8
   - Coronal_Ejections_Count: Set to 8
   - Activate_Surface_Rays: Set to true, for reasonable graphics settings;
     lower ones should have it disabled.

   OBAFGKM Color sets:

   Unless otherwise noted:
   Rays_Color = High_Zones_Color = Atmosphere_Color,
   Flares_Color = Ejections_Color = Explosions_Color

   O:
   - Atmosphere_Color: 0655FFFF
   - Explosions_Color: 0B64FFFF
   - Dark_Filaments_Color: 001B67FF
   - Low_Zones_Color: 0040FFFF

   (B is 2/3 of the way from F to O.)
   (A is 1/3 of the way from F to O.)

   F:
   - Atmosphere_Color: FFFFFFFF
   - Explosions_Color: 545454FF
   - Ejections_Color: FFFFFFFF
   - Dark_Filaments_Color: C5C5C5FF
   - Low_Zones_Color: 555555FF

   (G is 1/2 of the way from F to K.)

   K: (The defaults in the blueprint.)
   - Atmosphere_Color: FF3306FF
   - Explosions_Color: FF290BFF
   - Dark_Filaments_Color: 670600FF
   - Low_Zones_Color: FF1700FF

   M:
   - Atmosphere_Color: FF1F06FF
   - Explosions_Color: FF150BFF
   - Dark_Filaments_Color: 670300FF
   - Low_Zones_Color: FF0200FF


   Terran planet notes:
   - Axial tilt should be used to set the planet's rotation.

   - Use_Directional_Light: Set to true
   - Light_Source_Directional: Set to some directional light with rotation
     0,0,90
   - Night Brightness: Set to 0.01
   - Continents_Position: 0-15, linear dist
   - Continents_Spread: 1-10 dist: highest near 1, tapering to 10
   - Continents_Distortion: 0.5-1.5, normal dist
   - Continents_Distortion_Scale: 2-6, normal dist
   - Plains/Mountains_Transition: 0.5-1.5, normal dist
   - Plains/Mountains_Transition_Contrast: 0.5-1.5, normal dist

   TODO: For these four, it might be best to come up with some preset,
   self-consistent sets of four colors each.  (It may need to include cloud,
   atmosphere and city tints.) NOTE: This does not apply to habitable planets,
   but *does* apply to less-than-habitable ones).

   - Color_Mountains_{1,2}: Should be vaguely dun colored (default
     is linear E6C29EFF,74693AFF)
     Color_Mountains_1: color lerp between FFAF6FFF and 4A3320FF
     Color_Mountains_2: color lerp between 443D22FF and FFE67FFF

   - Color_Plains_{1,2}: Should be greenish (default is linear
     416B35FF,1F4C12FF)
     Color_Plains_1: color lerp between 77C360FF and 1C2D16FF
     Color_Plains_2: color lerp between 68FF3BFF and 16360DFF

   - T_Mountains: one of
     /Game/Space_Creator/PlanetCreator_1_V2/Textures/Color_Textures/T_PlanetTexture_Color_N.T_PlanetTexture_Color_N,
     where N is int [1,9].
   - T_Plains: one of the textures above; ok if they're the same.

   - Sea_Level: 0.0 - 1.0, should be based on %land.

   - Oceans_Color_Transition: 0.25-1, mode should be 0.5

   - Oceans_Color_{1,2,3}: Should be blue, obvs. (default is linear
     1A9B88FF,0C1C34FF,05070FFF)

   - Ice_Poles_Weight: 0.4-0.6, Ice_Coverage: 0.1-0.5 these should not be rng,
     but based on planet surface temp

   - Clouds_Speed: 0.0001-0.005, should be based on extreme weather properties
     of planet generation.  Leave it at the lowest value unless the planet has
     extreme weather.

   - Clouds_Opacity: 0.0-4.0, should be left at 2-4 unless there is low water;
     0-2 is only for arid planets

   - Clouds_Shadow_Offset: Set to 6.0

   - Under_Clouds_Brightness: Set to 0.5

   - Clouds_Color: should be whitish, but should also match the overall
     palette of the planet (default is 6D7884FF)

   - Clouds_Twilight_Color_1: should be slightly darker than Clouds_Color (use
     linear 5A5549FF; BP default is FFD167FF)

   - Clouds_Twilight_Color_2: this is the tint on the night-side of the
     day/night transition band (use linear 653602FF; BP default is DA3D0AFF)

   - Atmosphere_Direct_Brightness: 0-0.1 should be 0.1 for most planets; arid
     planets should be less (maybe scale this with 1-%land)

   - Atmosphere_Edge_Brightness: 0-1.0 should be 1.0 for most planets; arid
     planets should be less (maybe scale this with 1-%land)

   - Atmosphere_Color: Interpolate this between linear white and linear
     2F73E0FF, based on O2 suitability

   TODO: Rings!

   - City_Lights_Extent: 0.0-1.0, should be based on pop

   - City_Lights_Halo: Set to 0 for uncolonized, 0.005 otherwise

   - City_Lights_Intensity: 0.0-20.0, should be based on infrastructure

   - City_Lights_Color: Interpolate this between linear white and linear
     4EA7FFFF, based on O2 suitability
 */

/* Gas giant planet notes:
   - Global_Tile_Ratio: 0.1-0.9, linear distribution
   - Scattering_Color: TODO
   - Night_Color: Set to linear 02020300
   - Sunset_Color_1: TODO
   - Sunset_Color_2: TODO
   - Equator_Clouds_Color_{1,2,3,4}: TODO
   - Tropics_Clouds_Color_{1,2,3,4}: TODO
   - Deep_Clouds_Color_{1,2,3,4}: TODO
   - Poles_Color_{1,2,3,4}:TODO

   - Rings_Opacity: Set to 1.0 if rings are present, 0.0 otherwise.
   - Inner_Radius: 1.25-2.0, gamma dist (mode near 1.5)

   - Outer_Radius (actually used as thickness, not outer radius): 0.4-2.5,
     Inner_Radius implies higher Outer_Radius, but then add some linear
     dist-based randomness

   - Edge_Hardness: 0.75-20.0, linear dist
   - Frequency: 1.25-4.0, linear dist
   - Position: 0.0-10, linear dist
   - Dark_Side_Brightness: Set to 0.01
   - Rings_Color_{1,2,3}: TODO
   - Rings_Scattering_Color: Set to Rings_Color_3
 */

inline ADirectionalLight * directional_light(UWorld * w)
{
    check(w);
    for (TActorIterator<ADirectionalLight> it(w); it; ++it) {
        return *it;
    }
    throw std::runtime_error(
        "Could not get pointer to the current level's directional light.");
    return nullptr; // unreachable
}

template<typename T>
void set_property(AActor * a, FName name, T value)
{
    FProperty * const p = a->GetClass()->FindPropertyByName(name);
    if (!p) {
        throw std::runtime_error(std::format(
            "Actor {} does not have property {}.", a->GetName(), name));
    }

    T * const ptr = p->ContainerPtrToValuePtr<T>(a);
    if (!ptr) {
        throw std::runtime_error(std::format(
            "Could not get property {} of type {} from actor {}.",
            name,
            boost::typeindex::type_id<T>().pretty_name(),
            a->GetName()));
    }

    *ptr = std::move(value);
}

double seasons_intensity_factor(planet_t const & planet)
{
    using namespace adobe::literals;
    double const max_result = 3.0;
    double retval = 0.0;
    for (auto const & effect : planet.effects) {
        if (effect.target == planet_effect_target_t::growth_factor) {
            if (effect.name == "long_seasons"_name)
                retval += 1.0;
            else if (effect.name == "intense_seasons"_name)
                retval += 2.0;
        }
    }
    return retval / max_result;
}

// TODO: Move all the utilities above here somewhere else.

void configure_map_star(Amap_system * star_actor, system_t const & system)
{
    check(star_actor);

    star_t const & star = system.star;
    check(star_class_t::invalid_star_class < star.star_class);
    check(star.star_class <= star_class_t::m);

    auto const & materials = ::materials();
    UMaterialInterface * material = nullptr;
    switch (star.star_class) {
    case star_class_t::o: material = materials.blue_map_star_; break;
    case star_class_t::b:
    case star_class_t::a: material = materials.blue_white_map_star_; break;
    case star_class_t::f: material = materials.white_map_star_; break;
    case star_class_t::g:
    case star_class_t::k:
        material = random_int(0, 1) ? materials.yellow_map_star_0_
                                    : materials.yellow_map_star_1_;
        break;
    case star_class_t::m: {
        int const choice = random_int(0, 2);
        if (choice == 0)
            material = materials.red_map_star_0_;
        else if (choice == 1)
            material = materials.red_map_star_1_;
        else
            material = materials.red_map_star_2_;
        break;
    }
    default: break;
    }

    UMaterialInstanceDynamic * instance =
        UMaterialInstanceDynamic::Create(material, star_actor);

    {
        UTexture * texture = textures().random_small_lens_flare();
        instance->SetTextureParameterValue(TEXT("T_MainFlare"), texture);

#if 0 // TODO
        // https://en.wikipedia.org/wiki/Stellar_classification
        // Class A and brighter are >= 5x the sun.
        bool const highly_luminous = 5.0 < star.solar_luminosities;

        instance->SetScalarParameterValue(TEXT("Burst_Intensity"), TODO);
        instance->SetScalarParameterValue(TEXT("Halo_Intensity"), TODO);
        instance->SetScalarParameterValue(TEXT("Halo_SizeIntensity"), TODO);
#endif
    }

    star_actor->main_material(instance);

    // TODO: Need to fogure out how the halo part of the Space Creator
    // material works, at least enough to pull part of it out and use it for
    // these stars.
}

void configure_system_star(AActor * star_actor, star_t const & star)
{
    check(star_actor);
    check(star_class_t::invalid_star_class < star.star_class);
    check(star.star_class <= star_class_t::m);

    set_property(
        star_actor,
        TEXT("Atmospheric_Phenomena"),
        true); // TODO: Disable based on settings.
    set_property(star_actor, TEXT("Coronal_Rays_Count"), 128);
    set_property(star_actor, TEXT("Surface_Explosions_Count"), 8);
    set_property(star_actor, TEXT("Coronal_Ejections_Count"), 8);
    set_property(
        star_actor,
        TEXT("Activate_Surface_Rays"),
        true); // TODO: Disable based on settings.

    // class O
    FLinearColor const f_atmosphere_color =
        FLinearColor(0x06, 0x55, 0xFF, 0xFF);
    FLinearColor const f_explosions_color =
        FLinearColor(0x0B, 0x64, 0xFF, 0xFF);
    FLinearColor const f_ejections_color = f_explosions_color;
    FLinearColor const f_dark_filaments_color =
        FLinearColor(0x00, 0x1B, 0x67, 0xFF);
    FLinearColor const f_low_zones_color = FLinearColor(0x00, 0x40, 0xFF, 0xFF);

    // class F
    FLinearColor const o_atmosphere_color = FLinearColor::White;
    FLinearColor const o_explosions_color =
        FLinearColor(0x54, 0x54, 0x54, 0xFF);
    FLinearColor const o_ejections_color = FLinearColor::White;
    FLinearColor const o_dark_filaments_color =
        FLinearColor(0xC5, 0xC5, 0xC5, 0xFF);
    FLinearColor const o_low_zones_color = FLinearColor(0x55, 0x55, 0x55, 0xFF);

    // class K
    FLinearColor const k_atmosphere_color =
        FLinearColor(0xFF, 0x33, 0x06, 0xFF);
    FLinearColor const k_explosions_color =
        FLinearColor(0xFF, 0x29, 0x0B, 0xFF);
    FLinearColor const k_ejections_color = k_explosions_color;
    FLinearColor const k_dark_filaments_color =
        FLinearColor(0x67, 0x06, 0x00, 0xFF);
    FLinearColor const k_low_zones_color = FLinearColor(0xFF, 0x17, 0x00, 0xFF);

    FLinearColor atmosphere_color;
    FLinearColor explosions_color;
    FLinearColor ejections_color;
    FLinearColor dark_filaments_color;
    FLinearColor low_zones_color;
    switch (star.star_class) {
    case star_class_t::o:
        atmosphere_color = o_atmosphere_color;
        explosions_color = o_explosions_color;
        ejections_color = o_ejections_color;
        dark_filaments_color = o_dark_filaments_color;
        low_zones_color = o_low_zones_color;
        break;
    case star_class_t::b:
    case star_class_t::a: {
        double const alpha =
            (star.star_class == star_class_t::b ? 1.0 : 2.0) / 3.0;
        atmosphere_color = FLinearColor::LerpUsingHSV(
            f_atmosphere_color, o_atmosphere_color, alpha);
        ejections_color = explosions_color = FLinearColor::LerpUsingHSV(
            f_explosions_color, o_explosions_color, alpha);
        dark_filaments_color = FLinearColor::LerpUsingHSV(
            f_dark_filaments_color, o_dark_filaments_color, alpha);
        low_zones_color = FLinearColor::LerpUsingHSV(
            f_dark_filaments_color, o_dark_filaments_color, alpha);
        break;
    }
    case star_class_t::f:
        atmosphere_color = f_atmosphere_color;
        explosions_color = f_explosions_color;
        ejections_color = f_ejections_color;
        dark_filaments_color = f_dark_filaments_color;
        low_zones_color = f_low_zones_color;
        break;
    case star_class_t::g:
        atmosphere_color = FLinearColor::LerpUsingHSV(
            f_atmosphere_color, k_atmosphere_color, 0.5);
        ejections_color = explosions_color = FLinearColor::LerpUsingHSV(
            f_explosions_color, k_explosions_color, 0.5);
        dark_filaments_color = FLinearColor::LerpUsingHSV(
            f_dark_filaments_color, k_dark_filaments_color, 0.5);
        low_zones_color = FLinearColor::LerpUsingHSV(
            f_dark_filaments_color, k_dark_filaments_color, 0.5);
        break;
    case star_class_t::k:
        atmosphere_color = k_atmosphere_color;
        explosions_color = k_explosions_color;
        ejections_color = k_ejections_color;
        dark_filaments_color = k_dark_filaments_color;
        low_zones_color = k_low_zones_color;
        break;
    case star_class_t::m:
        atmosphere_color = FLinearColor(0xFF, 0x1F, 0x06, 0xFF);
        ejections_color = explosions_color =
            FLinearColor(0xFF, 0x15, 0x0B, 0xFF);
        dark_filaments_color = FLinearColor(0x67, 0x03, 0x00, 0xFF);
        low_zones_color = FLinearColor(0xFF, 0x02, 0x00, 0xFF);
        break;
    default: break;
    }
    set_property(star_actor, TEXT("Atmosphere_Color"), atmosphere_color);
    set_property(star_actor, TEXT("Explosions_Color"), explosions_color);
    set_property(star_actor, TEXT("Ejections_Color"), ejections_color);
    set_property(
        star_actor, TEXT("Dark_Filaments_Color"), dark_filaments_color);
    set_property(star_actor, TEXT("Flares_Color"), explosions_color);
    set_property(star_actor, TEXT("High_Zones_Color"), atmosphere_color);
    set_property(star_actor, TEXT("Low_Zones_Color"), low_zones_color);
    set_property(star_actor, TEXT("Rays_Color"), atmosphere_color);
}

// TODO: Aplanet_actor?
// TODO: For very cold planets that get terraformed, use the BP_Planet_Ice
// blueprint from Space_Creator, and reduce the ice over time, as the planet
// is terraformed.  After the terraforming has gotten close enough to
// Earthlike conditions common, change it to use the BP_Planet_Terran
// blueprint, keeping the properties that they have in common.
void configure_rocky_oxidized_planet(
    AActor * planet_actor, planet_t const & planet)
{
    check(planet_actor);

    FQuat tilt_rot(
        FVector(1, 0, 0), FMath::DegreesToRadians(planet.axial_tilt_d));
    // Rotation, relative to the directional light, based on the planet's
    // current position within its orbit.  This show each planet as if viewed
    // from infinitely far away, in the -y direction, looking in the +y
    // direction.
    FQuat rot_from_position(FVector(0, 0, 1), -planet.orbital_pos_r);
    planet_actor->SetActorRotation(tilt_rot * rot_from_position);

    // fixed values

    // Global
    ADirectionalLight * const light =
        directional_light(planet_actor->GetWorld());

    // TODO: Need a graphics setting for this.
    set_property(planet_actor, TEXT("Shader_Complexity"), 4);
    set_property(planet_actor, TEXT("Use_Directional_Light"), true);
    set_property(planet_actor, TEXT("Use_Directional_Light"), light);
    set_property(planet_actor, TEXT("Night_Brightness"), 0.01);

    // computed values

    // TODO: Move these out of this function?
    std::gamma_distribution<double> one_to_ten_gamma_dist(1, 4);
    std::normal_distribution<double> around_one_dist(0.5, 1.5);
    std::normal_distribution<double> distortion_scale_dist(2.0, 6.0);
    std::chi_squared_distribution oceans_transition_dist(2.5);

    // Continents
    set_property(
        planet_actor, TEXT("Continents_Position"), random_double(0.0, 15.0));
    set_property(
        planet_actor,
        TEXT("Continents_Spread"),
        std::clamp(1.0, 10.0, random_number(one_to_ten_gamma_dist)));
    set_property(
        planet_actor,
        TEXT("Continents_Distortion"),
        random_number(around_one_dist));
    set_property(
        planet_actor,
        TEXT("Continents_Distortion_Scale"),
        random_number(distortion_scale_dist));
    set_property(
        planet_actor,
        TEXT("Plains/Mountains_Transition"),
        random_number(around_one_dist));
    set_property(
        planet_actor,
        TEXT("Plains/Mountains_Transition_Contrast"),
        random_number(around_one_dist));

    {
        auto const min = FLinearColor(0xFF, 0xAF, 0x6F, 0xFF);
        auto const max = FLinearColor(0x4A, 0x33, 0x20, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Mountains_1"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }
    {
        auto const min = FLinearColor(0x44, 0x3D, 0x22, 0xFF);
        auto const max = FLinearColor(0xFF, 0xE6, 0x7F, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Mountains_2"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }
    {
        auto const min = FLinearColor(0x77, 0xC3, 0x60, 0xFF);
        auto const max = FLinearColor(0x1C, 0x2D, 0x16, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Plains_1"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }
    {
        auto const min = FLinearColor(0x68, 0xFF, 0x3B, 0xFF);
        auto const max = FLinearColor(0x16, 0x36, 0x0D, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Plains_2"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }

    set_property(
        planet_actor, TEXT("T_Mountains"), textures().random_planet_texture());
    set_property(
        planet_actor, TEXT("T_Plains"), textures().random_planet_texture());

    // oceans
    set_property(planet_actor, TEXT("Sea_Level"), planet.ocean_coverage);

    set_property(
        planet_actor,
        TEXT("Oceans_Color_Transition"),
        std::lerp(
            0.25,
            1.0,
            random_number(oceans_transition_dist) /
                oceans_transition_dist.max()));

    {
        auto const min = FLinearColor(0x2C, 0xFF, 0xE5, 0xFF);
        auto const max = FLinearColor(0x05, 0x1D, 0x1A, 0xFF);
        set_property(
            planet_actor,
            TEXT("Oceans_Color_1"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }
    {
        auto const min = FLinearColor(0x02, 0x04, 0x07, 0xFF);
        auto const max = FLinearColor(0x3A, 0x88, 0xFF, 0xFF);
        set_property(
            planet_actor,
            TEXT("Oceans_Color_2"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }
    {
        auto const min = FLinearColor(0x05, 0x07, 0x0F, 0xFF);
        auto const max = FLinearColor(0x28, 0x3D, 0x82, 0xFF);
        set_property(
            planet_actor,
            TEXT("Oceans_Color_3"),
            FLinearColor::LerpUsingHSV(min, max, random_unit_double()));
    }

    // ice
    double const temperature_alpha =
        (planet.surface_temperature_k - min_habitable_nonsuit_temp_k) /
        (max_habitable_temp_k - min_habitable_nonsuit_temp_k);
    set_property(
        planet_actor,
        TEXT("Ice_Poles_Weight"),
        std::max(0.0, std::lerp(0.4, 0.6, 1.0 - temperature_alpha)));
    set_property(
        planet_actor,
        TEXT("Ice_Coverage"),
        std::max(0.0, std::lerp(0.1, 0.5, 1.0 - temperature_alpha)));

    // clouds
    double clouds_speed = 0.0001;
    if (double alpha = seasons_intensity_factor(planet))
        clouds_speed = std::lerp(0.0001, 0.005, alpha);
    set_property(planet_actor, TEXT("Clouds_Speed"), clouds_speed);

    double const plentiful_water_alpha =
        growth_factor_considered_habitable < planet.growth_factor
            ? 1.0
            : double(planet.water) / max_resource_value;

    set_property(
        planet_actor,
        TEXT("Clouds_Opacity"),
        plentiful_water_alpha < 1.0 ? random_double(0.0, 2.0)
                                    : random_double(2.0, 4.0));
    set_property(planet_actor, TEXT("Clouds_Shadow_Offset"), 6.0);
    set_property(planet_actor, TEXT("Under_Clouds_Brightness"), 0.5);
    set_property(
        planet_actor,
        TEXT("Clouds_Twilight_Color_1"),
        FLinearColor(0x5A, 0x55, 0x49, 0xFF));
    set_property(
        planet_actor,
        TEXT("Clouds_Twilight_Color_2"),
        FLinearColor(0x65, 0x36, 0x02, 0xFF));

    // atmosphere

    set_property(
        planet_actor,
        TEXT("Atmosphere_Direct_Brightness"),
        0.1 * plentiful_water_alpha);
    set_property(
        planet_actor,
        TEXT("Atmosphere_Edge_Brightness"),
        1.0 * plentiful_water_alpha);
    {
        auto const min = FLinearColor::White;
        auto const max = FLinearColor(0x2F, 0x73, 0xE0, 0xFF);
        set_property(
            planet_actor,
            TEXT("Atmosphere_Color"),
            FLinearColor::LerpUsingHSV(min, max, planet.o2_co2_suitability));
    }

    // TODO: Rings!

    // city lights
    if (planet.population <= 0.0f) {
        set_property(planet_actor, TEXT("City_Lights_Extent"), 0.0);
        set_property(planet_actor, TEXT("City_Lights_Halo"), 0.0);
        return;
    }
    set_property(
        planet_actor,
        TEXT("City_Lights_Extent"),
        double(planet.population / planet.max_population));
    set_property(planet_actor, TEXT("City_Lights_Halo"), 0.005);
    set_property(
        planet_actor,
        TEXT("City_Lights_Intensity"),
        std::lerp(0.0, 20.0, planet.infrastructure / max_infrastructure));
    {
        auto const min = FLinearColor::White;
        auto const max = FLinearColor(0x4E, 0xA7, 0xFF, 0xFF);
        set_property(
            planet_actor,
            TEXT("City_Lights_Color"),
            FLinearColor::LerpUsingHSV(min, max, planet.o2_co2_suitability));
    }
}

void configure_rocky_reduced_or_carbon_rich_planet(
    AActor * planet_actor, planet_t const & planet)
{
    check(planet_actor);

    // TODO
}

void configure_high_temperature_planet(
    AActor * planet_actor, planet_t const & planet)
{
    check(planet_actor);

    // TODO
}

void configure_gas_giant_planet(AActor * planet_actor, planet_t const & planet)
{
    check(planet_actor);

    // TODO
}

void configure_ice_giant_planet(AActor * planet_actor, planet_t const & planet)
{
    check(planet_actor);

    // TODO
}
