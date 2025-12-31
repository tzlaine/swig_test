#include "space_creator_actor_config.hpp"

#include "Aplayer_controller.h"
#include "Amap_system.h"
#include "constants.hpp"
#include "game_user_settings.h"
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
   - Scattering_Color: AA907B00 TODO
   - Night_Color: Set to linear 0.001 each channel
   - Sunset_Color_1: 8CAFFFFF TODO
   - Sunset_Color_2: FD9500FF
   - Equator_Clouds_Color_{1,2,3,4}: TODO
   - Tropics_Clouds_Color_{1,2,3,4}: TODO
   - Deep_Clouds_Color_{1,2,3,4}: TODO
   - Poles_Color_{1,2,3,4}:TODO

   - Equator Clouds Color Shift: 0-1.6 linear
   - Equator Clouds Color Uniformity: 0-4 linear
   - Equator Clouds Shadows Size: 0-0.07 normal
   - Equator Clouds Shadows Strength: 0-4 normal

   - Tropics *: same as above

   - Poles Latitude: 0.5

   - Rings_Opacity: Set to 1.0-8.0 linear if rings are present, 0.0 otherwise.
   - Inner_Radius: 1.25-2.0, gamma dist (mode near 1.5)

   - Outer_Radius (actually used as thickness, not outer radius): 0.4-2.5,
     Inner_Radius implies higher Outer_Radius, but then add some linear
     dist-based randomness

   - Edge_Hardness: 0.75-20.0, linear dist
   - Frequency: 1.25-4.0, linear dist
   - Position: 0.0-10, linear dist
   - Dark_Side_Brightness: Set to 0.001
   - Rings_Color_{1,2,3}: TODO (mostly ice; should be very desaturated, like
   s=0.4-0.45)
   - Rings_Scattering_Color: Set to Rings_Color_3
 */

/* reduced/carbon-rich planet notes:
   - Shader_Complexity: floor(planet shader complexity * 3.0 / 4.0 + 0.5)

   - Day_Brightness: 1.0
   - Night_Brightness: 0.001

   - Continents_Color_{1,2,3,4}: TODO
   _ Continents_Color_Overlay: TODO

   - {Large,Medium,Small}_Craters_Intensity: 0-2.0 linear for reduced; 0-1.0
   linear for carbon-rich
   - Craters_Normal_Intensity: 2.0 for reduced; 0-1.0 linear for carbon-rich

   - Clouds_Opacity: 0 for reduced; 0-0.8 linear for carbon-rich
   - Clouds_Shadow_Offset: 6.0
   - Under_Clouds_Brightness: 0.5

   - Atmosphere_Direct_Brightness: 0 for reduced; 0.01-0.05 for carbon-rich
   - Atmosphere_Edge_Brightness: 0-0.05 for reduced; 0.05-0.1 for carbon-rich
   - Atmosphere_Exponent: 3.0-6.0 for reduced; 0-5.0 for carbon-rich
   - Atmosphere_Color: white
   - Twilight_Color_1: white
   - Twilight_Color_2: white
 */

namespace {
    FVector light_dir(planet_t const & planet)
    {
        FVector retval{};
        FMath::PolarToCartesian(
            1.0, 1.0 * planet.orbital_pos_r, retval.X, retval.Y);
        return retval;
    }

    double seasons_intensity_factor(planet_t const & planet)
    {
        using namespace adobe::literals;
        double const max_result = 3.0;
        double retval = 0.0;
        for (auto const & effect : planet.effects) {
            if (effect.name == "long_seasons"_name)
                retval += 1.0;
            else if (effect.name == "intense_seasons"_name)
                retval += 2.0;
        }
        return retval / max_result;
    }
}

Fsystem_graphical_properties generate_map_star(system_t const & system)
{
    Fsystem_graphical_properties retval;

    star_t const & star = system.star;
    check(star_class_t::invalid_star_class < star.star_class);
    check(star.star_class <= star_class_t::m);

    retval.star_class = (int32)star.star_class;

    switch (star.star_class) {
    case star_class_t::o:
    case star_class_t::b:
    case star_class_t::a:
    case star_class_t::f:
    case star_class_t::g: break;
    case star_class_t::k: retval.material_index = random_int(0, 1); break;
    case star_class_t::m: retval.material_index = random_int(0, 2); break;
    default: break;
    }

    retval.lens_flare_index = textures().random_small_lens_flare();

    return retval;
}

void configure_map_star(
    Amap_system & system,
    Fsystem_graphical_properties const & props,
    bool capitol)
{
    auto const & materials = ::materials();
    UMaterialInterface * material = nullptr;
    switch ((star_class_t)props.star_class) {
    case star_class_t::o: material = materials.blue_map_star_; break;
    case star_class_t::b:
    case star_class_t::a: material = materials.blue_white_map_star_; break;
    case star_class_t::f: material = materials.white_map_star_; break;
    case star_class_t::g:
    case star_class_t::k:
        material = props.material_index ? materials.yellow_map_star_0_
                                        : materials.yellow_map_star_1_;
        break;
    case star_class_t::m:
        if (props.material_index == 0)
            material = materials.red_map_star_0_;
        else if (props.material_index == 1)
            material = materials.red_map_star_1_;
        else
            material = materials.red_map_star_2_;
        break;
    default: break;
    }

    UMaterialInstanceDynamic * instance =
        UMaterialInstanceDynamic::Create(material, &system);

    {
        UTexture * texture =
            textures().small_lens_flare(props.lens_flare_index);
        instance->SetTextureParameterValue(TEXT("T_MainFlare"), texture);

        // TODO
#if 0
        // https://en.wikipedia.org/wiki/Stellar_classification
        // Class A and brighter are >= 5x the sun.
        bool const highly_luminous = 5.0 < star.solar_luminosities;

        instance->SetScalarParameterValue(TEXT("Burst_Intensity"), TODO);
        instance->SetScalarParameterValue(TEXT("Halo_Intensity"), TODO);
        instance->SetScalarParameterValue(TEXT("Halo_SizeIntensity"), TODO);
#endif
    }

    system.main_material(instance);

    UMaterialInstanceDynamic * selected_mid = UMaterialInstanceDynamic::Create(
        capitol ? materials.system_star_outline_ : materials.system_selected_,
        &system);
    selected_mid->SetVectorParameterValue(
        TEXT("color"), ui_defaults().system_selected_color_);
    UMaterialInstanceDynamic * hovered_mid = UMaterialInstanceDynamic::Create(
        capitol ? materials.system_star_outline_ : materials.system_selected_,
        &system);
    hovered_mid->SetVectorParameterValue(
        TEXT("color"), ui_defaults().system_hovered_color_);
    system.selection_materials(selected_mid, hovered_mid);
}

// TODO: Such simple graphical settings should go into BPs instead.  This
// should be replaced by an array of each stellar class in the player
// controller.  The particular one used should be chosen at random.
void configure_system_star(AActor * star_actor, star_t const & star)
{
    check(star_actor);
    check(star_class_t::invalid_star_class < star.star_class);
    check(star.star_class <= star_class_t::m);

    set_property(
        star_actor,
        TEXT("Atmospheric Phenomena"),
        true); // TODO: Disable based on settings.
    set_property(star_actor, TEXT("Coronal Rays Count"), 128);
    set_property(star_actor, TEXT("Surface Explosions Count"), 8);
    set_property(star_actor, TEXT("Coronal Ejections Count"), 8);
    set_property(
        star_actor,
        TEXT("Activate Surface Rays"),
        true); // TODO: Disable based on settings.

    // class O
    FLinearColor const o_atmosphere_color = FColor(0x06, 0x55, 0xFF, 0xFF);
    FLinearColor const o_explosions_color = FColor(0x0B, 0x64, 0xFF, 0xFF);
    FLinearColor const o_ejections_color = o_explosions_color;
    FLinearColor const o_dark_filaments_color = FColor(0x00, 0x1B, 0x67, 0xFF);
    FLinearColor const o_low_zones_color = FColor(0x00, 0x40, 0xFF, 0xFF);

    // class F
    FLinearColor const f_atmosphere_color = FColor::White;
    FLinearColor const f_explosions_color = FColor(0x54, 0x54, 0x54, 0xFF);
    FLinearColor const f_ejections_color = FColor::White;
    FLinearColor const f_dark_filaments_color = FColor(0xC5, 0xC5, 0xC5, 0xFF);
    FLinearColor const f_low_zones_color = FColor(0x55, 0x55, 0x55, 0xFF);

    // class K
    FLinearColor const k_atmosphere_color = FColor(0xFF, 0x33, 0x06, 0xFF);
    FLinearColor const k_explosions_color = FColor(0xFF, 0x29, 0x0B, 0xFF);
    FLinearColor const k_ejections_color = k_explosions_color;
    FLinearColor const k_dark_filaments_color = FColor(0x67, 0x06, 0x00, 0xFF);
    FLinearColor const k_low_zones_color = FColor(0xFF, 0x17, 0x00, 0xFF);

    FLinearColor atmosphere_color = {};
    FLinearColor explosions_color = {};
    FLinearColor ejections_color = {};
    FLinearColor dark_filaments_color = {};
    FLinearColor low_zones_color = {};
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
        atmosphere_color =
            f_atmosphere_color * (1 - alpha) + o_atmosphere_color * alpha;
        ejections_color = explosions_color =
            f_explosions_color * (1 - alpha) + o_explosions_color * alpha;
        dark_filaments_color = f_dark_filaments_color * (1 - alpha) +
                               o_dark_filaments_color * alpha;
        low_zones_color = f_dark_filaments_color * (1 - alpha) +
                          o_dark_filaments_color * alpha;
        break;
    }
    case star_class_t::f:
        atmosphere_color = f_atmosphere_color;
        explosions_color = f_explosions_color;
        ejections_color = f_ejections_color;
        dark_filaments_color = f_dark_filaments_color;
        low_zones_color = f_low_zones_color;
        break;
    case star_class_t::g: set_property(star_actor, TEXT("dirty"), true); return;
    case star_class_t::k:
        atmosphere_color = k_atmosphere_color;
        explosions_color = k_explosions_color;
        ejections_color = k_ejections_color;
        dark_filaments_color = k_dark_filaments_color;
        low_zones_color = k_low_zones_color;
        break;
    case star_class_t::m:
        atmosphere_color = FColor(0xFF, 0x1F, 0x06, 0xFF);
        ejections_color = explosions_color = FColor(0xFF, 0x15, 0x0B, 0xFF);
        dark_filaments_color = FColor(0x67, 0x03, 0x00, 0xFF);
        low_zones_color = FColor(0xFF, 0x02, 0x00, 0xFF);
        break;
    default: break;
    }
    set_property(star_actor, TEXT("Atmosphere Color"), atmosphere_color);
    set_property(star_actor, TEXT("Explosions Color"), explosions_color);
    set_property(star_actor, TEXT("Ejections Color"), ejections_color);
    set_property(
        star_actor, TEXT("Dark Filaments Color"), dark_filaments_color);
    set_property(star_actor, TEXT("Flares Color"), explosions_color);
    set_property(star_actor, TEXT("High Zones Color"), atmosphere_color);
    set_property(star_actor, TEXT("Low Zones Color"), low_zones_color);
    set_property(star_actor, TEXT("Rays Color"), atmosphere_color);

    set_property(star_actor, TEXT("dirty"), true);
}

// TODO: For very cold planets that get terraformed, use the BP_Planet_Ice
// blueprint from Space_Creator, and reduce the ice over time, as the planet
// is terraformed.  After the terraforming has gotten close enough to
// Earthlike conditions common, change it to use the BP_Planet_Terran
// blueprint, keeping the properties that they have in common.
void configure_rocky_oxidized_planet(
    AActor * planet_actor,
    planet_t const & planet,
    int planet_id,
    float population,
    float infrastructure)
{
    check(planet_actor);

    Ugame_user_settings * game_user_settings = Ugame_user_settings::get();
    check(game_user_settings);

    // TODO: Also use a global seed so that a particular system+planet (say,
    // system_id=3 and planet_id=4) doesn't always looks the same, game after
    // game.
    auto rng_state = detail::rng_state_from(planet.system_id, planet_id);

    set_property(
        planet_actor,
        TEXT("Shader_Complexity"),
        game_user_settings->planet_detail);
    set_property(planet_actor, TEXT("Use_Directional_Light"), false);
    set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
    set_property(planet_actor, TEXT("Night_Brightness"), 0.001f);
    set_property(planet_actor, TEXT("Day_Brightness"), 1.0f);
    // TODO: Vary with distance to star?

    // computed values

    std::gamma_distribution<double> one_to_ten_gamma_dist(1, 4);
    std::normal_distribution<double> around_one_dist(0.5, 1.5);
    std::normal_distribution<double> distortion_scale_dist(2.0, 6.0);
    std::chi_squared_distribution oceans_transition_dist(2.5);

    // Continents
    set_property(
        planet_actor,
        TEXT("Continents_Position"),
        random_double(0.0, 15.0, rng_state));
    set_property(
        planet_actor,
        TEXT("Continents_Spread"),
        std::clamp(1.0, 10.0, random_number(one_to_ten_gamma_dist, rng_state)));
    set_property(
        planet_actor,
        TEXT("Continents_Distortion"),
        random_number(around_one_dist, rng_state));
    set_property(
        planet_actor,
        TEXT("Continents_Distortion_Scale"),
        random_number(distortion_scale_dist, rng_state));
    set_property(
        planet_actor,
        TEXT("Plains/Mountains_Transition"),
        random_number(around_one_dist, rng_state));
    set_property(
        planet_actor,
        TEXT("Plains/Mountains_Transition_Contrast"),
        random_number(around_one_dist, rng_state));

    {
        FLinearColor const min = FColor(0xFF, 0xAF, 0x6F, 0xFF);
        FLinearColor const max = FColor(0x4A, 0x33, 0x20, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Mountains_1"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }
    {
        FLinearColor const min = FColor(0x44, 0x3D, 0x22, 0xFF);
        FLinearColor const max = FColor(0xFF, 0xE6, 0x7F, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Mountains_2"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }
    {
        FLinearColor const min = FColor(0x77, 0xC3, 0x60, 0xFF);
        FLinearColor const max = FColor(0x1C, 0x2D, 0x16, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Plains_1"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }
    {
        FLinearColor const min = FColor(0x68, 0xFF, 0x3B, 0xFF);
        FLinearColor const max = FColor(0x16, 0x36, 0x0D, 0xFF);
        set_property(
            planet_actor,
            TEXT("Color_Plains_2"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }

    set_property(
        planet_actor,
        TEXT("T_Mountains"),
        textures().random_planet_texture(rng_state));
    set_property(
        planet_actor,
        TEXT("T_Plains"),
        textures().random_planet_texture(rng_state));

    // oceans
    set_property(planet_actor, TEXT("Sea_Level"), planet.ocean_coverage);

    set_property(
        planet_actor,
        TEXT("Oceans_Color_Transition"),
        std::lerp(
            0.25,
            1.0,
            random_number(oceans_transition_dist, rng_state) /
                oceans_transition_dist.max()));

    {
        FLinearColor const min = FColor(0x2C, 0xFF, 0xE5, 0xFF);
        FLinearColor const max = FColor(0x05, 0x1D, 0x1A, 0xFF);
        set_property(
            planet_actor,
            TEXT("Oceans_Color_1"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }
    {
        FLinearColor const min = FColor(0x02, 0x04, 0x07, 0xFF);
        FLinearColor const max = FColor(0x3A, 0x88, 0xFF, 0xFF);
        set_property(
            planet_actor,
            TEXT("Oceans_Color_2"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }
    {
        FLinearColor const min = FColor(0x05, 0x07, 0x0F, 0xFF);
        FLinearColor const max = FColor(0x28, 0x3D, 0x82, 0xFF);
        set_property(
            planet_actor,
            TEXT("Oceans_Color_3"),
            FLinearColor::LerpUsingHSV(
                min, max, random_unit_double(rng_state)));
    }

    // ice
    double const temperature_alpha =
        (planet.surface_temperature_k - min_habitable_nonsuit_temp_k) /
        (max_habitable_temp_k - min_habitable_nonsuit_temp_k);
    set_property(
        planet_actor,
        TEXT("Ice_Poles_Weight"),
        std::max(0.0, std::lerp(0.6, 0.7, 1.0 - temperature_alpha)));
    set_property(
        planet_actor,
        TEXT("Ice_Coverage"),
        std::max(0.0, std::lerp(0.0, 0.4, 1.0 - temperature_alpha)));

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
        plentiful_water_alpha < 1.0 ? random_double(0.0, 2.0, rng_state)
                                    : random_double(2.0, 4.0, rng_state));
    set_property(planet_actor, TEXT("Clouds_Shadow_Offset"), 6.0);
    set_property(planet_actor, TEXT("Under_Clouds_Brightness"), 0.5);

    set_property(
        planet_actor,
        TEXT("Clouds_Twilight_Color_1"),
        FLinearColor(FColor(0xFF, 0xEE, 0xBE, 0xFF)));
    set_property(
        planet_actor,
        TEXT("Clouds_Twilight_Color_2"),
        FLinearColor(FColor(0xEE, 0xB6, 0x9B, 0xFF)));

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
        FLinearColor const min = FColor::White;
        FLinearColor const max = FColor(0x2F, 0x73, 0xE0, 0xFF);
        set_property(
            planet_actor,
            TEXT("Atmosphere_Color"),
            FLinearColor::LerpUsingHSV(min, max, planet.o2_co2_suitability));
    }

    // TODO: Rings!

    // city lights
    if (population <= 0.0f) {
        set_property(planet_actor, TEXT("CityLights_Extent"), 0.0f);
        set_property(planet_actor, TEXT("CityLights_Halo"), 0.0f);
    } else {
        set_property(
            planet_actor,
            TEXT("CityLights_Extent"),
            population / planet.max_population);
        set_property(planet_actor, TEXT("CityLights_Halo"), 0.005f);
        set_property(
            planet_actor,
            TEXT("CityLights_Intensity"),
            std::lerp(0.0f, 20.0f, infrastructure / max_infrastructure));
        // TODO: Passing max pop and infra to this function seems to result in
        // half, not max, values (0.5 extent and 10 intensity).  Fix.
        {
            FLinearColor const min = FColor::White;
            FLinearColor const max = FColor(0x4E, 0xA7, 0xFF, 0xFF);
            set_property(
                planet_actor,
                TEXT("CityLights_Color"),
                FLinearColor::LerpUsingHSV(
                    min, max, planet.o2_co2_suitability));
        }
    }

    set_property(planet_actor, TEXT("dirty"), true);
}

void configure_rocky_reduced_or_carbon_rich_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    Ugame_user_settings * game_user_settings = Ugame_user_settings::get();
    check(game_user_settings);

    auto rng_state = detail::rng_state_from(planet.system_id, planet_id);

    int complexity = game_user_settings->planet_detail;
    if (complexity == 3)
        complexity = 2;
    else if (complexity == 4)
        complexity = 3;
    set_property(planet_actor, TEXT("Shader_Complexity"), complexity);
    set_property(planet_actor, TEXT("Use_Directional_Light"), false);
    set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
    set_property(planet_actor, TEXT("Night_Brightness"), 0.001f);
    set_property(planet_actor, TEXT("Day_Brightness"), 1.0f);

    set_property(
        planet_actor,
        TEXT("Continenets_Texture_1"),
        textures().random_planet_texture(rng_state));
    set_property(
        planet_actor,
        TEXT("Continenets_Texture_2"),
        textures().random_planet_texture(rng_state));

    // The colors here are from the Space Creator defaults, which are Mars
    // like, redish-brown.  Reduced-atmosphere planets should not have a lot
    // of surface chemistry, and so should be grey.  Carbon-rich atmosphere
    // planets will be somewhere between grey and MArs-like, weighted hevily
    // toward grey (hence the gamma distribution).
    std::gamma_distribution<double> saturation_dist(1, 2);
    double const desaturation =
        planet.atmosphere_type == atmosphere_type_t::reduced_type_a
            ? 1.0
            : std::clamp(
                  0.0,
                  1.0,
                  1 - random_number(saturation_dist, rng_state) / 2.0);
    FLinearColor const continents_color_1 =
        FLinearColor(FColor(0xF1, 0x77, 0x55, 0xFF)).Desaturate(desaturation);
    FLinearColor const continents_color_2 =
        FLinearColor(FColor(0xFF, 0x80, 0x54, 0xFF)).Desaturate(desaturation);
    FLinearColor const continents_color_3 =
        FLinearColor(FColor(0xFF, 0xA6, 0x67, 0xFF)).Desaturate(desaturation);
    FLinearColor const continents_color_4 =
        FLinearColor(FColor(0xD3, 0xE2, 0xED, 0xFF)).Desaturate(desaturation);
    FLinearColor const continents_color_overlay =
        FLinearColor(FColor(0xA0, 0x49, 0x40, 0xFF)).Desaturate(desaturation);
    set_property(planet_actor, TEXT("Continents_Color_1"), continents_color_1);
    set_property(planet_actor, TEXT("Continents_Color_2"), continents_color_2);
    set_property(planet_actor, TEXT("Continents_Color_3"), continents_color_3);
    set_property(planet_actor, TEXT("Continents_Color_4"), continents_color_4);
    set_property(
        planet_actor,
        TEXT("Continents_Color_Overlay"),
        continents_color_overlay);

    if (planet.atmosphere_type == atmosphere_type_t::reduced_type_a) {
        set_property(
            planet_actor,
            TEXT("Large_Craters_Intensity"),
            random_double(0.0, 2.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Medium_Craters_Intensity"),
            random_double(0.0, 2.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Small_Craters_Intensity"),
            random_double(0.0, 2.0, rng_state));
        set_property(planet_actor, TEXT("Craters_Normal_Intensity"), 2.0f);
        set_property(planet_actor, TEXT("Clouds_Opacity"), 0.0f);
        set_property(planet_actor, TEXT("Atmosphere_Direct_Brightness"), 0.0f);
        set_property(
            planet_actor,
            TEXT("Atmosphere_Edge_Brightness"),
            random_double(0.0, 0.05, rng_state));
        set_property(
            planet_actor,
            TEXT("Atmosphere_Exponent"),
            random_double(3.0, 6.0, rng_state));
    } else {
        set_property(
            planet_actor,
            TEXT("Large_Craters_Intensity"),
            random_double(0.0, 1.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Medium_Craters_Intensity"),
            random_double(0.0, 1.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Small_Craters_Intensity"),
            random_double(0.0, 1.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Craters_Normal_Intensity"),
            random_double(0.0, 1.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Clouds_Opacity"),
            random_double(0.0, 0.8, rng_state));
        set_property(
            planet_actor,
            TEXT("Atmosphere_Direct_Brightness"),
            random_double(0.01, 0.05, rng_state));
        set_property(
            planet_actor,
            TEXT("Atmosphere_Edge_Brightness"),
            random_double(0.05, 0.1, rng_state));
        set_property(
            planet_actor,
            TEXT("Atmosphere_Exponent"),
            random_double(0.0, 5.0, rng_state));
    }

    set_property(planet_actor, TEXT("Clouds_Shadow_Offset"), 6.0f);
    set_property(planet_actor, TEXT("Under_Clouds_Brightness"), 0.5f);
    set_property(planet_actor, TEXT("Atmosphere_Color"), FLinearColor::White);
    set_property(planet_actor, TEXT("Twilight_Color_1"), FLinearColor::White);
    set_property(planet_actor, TEXT("Twilight_Color_2"), FLinearColor::White);

    // TODO: Rings!

    set_property(planet_actor, TEXT("dirty"), true);
}

void configure_high_temperature_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    Ugame_user_settings * game_user_settings = Ugame_user_settings::get();
    check(game_user_settings);

    set_property(
        planet_actor,
        TEXT("Shader_Complexity"),
        game_user_settings->planet_detail);
    set_property(planet_actor, TEXT("Use_Directional_Light"), false);
    set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
    set_property(planet_actor, TEXT("Night_Brightness"), 0.001f);
    set_property(planet_actor, TEXT("Day_Brightness"), 1.0f);

    // TODO

    set_property(planet_actor, TEXT("dirty"), true);
}

void configure_gas_giant_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
    set_property(
        planet_actor, TEXT("Night Color"), FLinearColor(FVector(0.001)));
    set_property(planet_actor, TEXT("Dark_Side_Brightness"), 0.001f);

    // TODO

    set_property(planet_actor, TEXT("dirty"), true);
}

void configure_ice_giant_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
    set_property(
        planet_actor, TEXT("Night Color"), FLinearColor(FVector(0.001)));
    set_property(planet_actor, TEXT("Dark_Side_Brightness"), 0.001f);

    // TODO

    set_property(planet_actor, TEXT("dirty"), true);
}
