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

   - City_Lights_Extent: 0.0-1.0, should be based on pop

   - City_Lights_Halo: Set to 0 for uncolonized, 0.005 otherwise

   - City_Lights_Intensity: 0.0-20.0, should be based on infrastructure

   - City_Lights_Color: Interpolate this between linear white and linear
     4EA7FFFF, based on O2 suitability
 */

/* Gas giant planet notes:
   - Global_Tile_Ratio: 0.1-0.9, linear distribution
   - Scattering_Color: [from color set]
   - Night_Color: Set to linear 0.001 each channel
   - Sunset_Color_1: [from color set]
   - Sunset_Color_2: [from color set]
   - Equator_Clouds_Color_{1,2,3,4}: [from color set]
   - Tropics_Clouds_Color_{1,2,3,4}: [from color set]
   - Deep_Clouds_Color_{1,2,3,4}: [from color set]
   - Poles_Color_{1,2,3,4}: [from color set]

   - Equator Clouds Color Shift: 0-1.6 linear
   - Equator Clouds Color Uniformity: 0-4 linear
   - Equator Clouds Shadows Size: 0-0.07 normal
   - Equator Clouds Shadows Strength: 0-4 normal

   - Tropics *: same as above

   - Poles Latitude: 0.2-0.5 linear
 */

/* rings (for all planet types) notes:
   - Rings_Opacity: Set to 1.0-8.0 linear if rings are present, 0.0 otherwise.
   - Inner_Radius: 1.25-2.0, linear dist

   - Outer_Radius (actually used as thickness, not outer radius): 0.4-2.5,
     Inner_Radius implies higher Outer_Radius, but then add some linear
     dist-based randomness

   - Edge_Hardness: 0.75-20.0, linear dist
   - Frequency: 1.25-4.0, linear dist
   - Position: 0.0-10, linear dist
   - Rings_Color_{1,2,3}: [from color set] (mostly ice; should be very desaturated, like
   s=0.4-0.45)
   - Rings_Scattering_Color: [from color set]
 */

/* reduced/carbon-rich planet notes:
   - Shader_Complexity: floor(planet shader complexity * 3.0 / 4.0 + 0.5)

   - Day_Brightness: 1.0
   - Night_Brightness: 0.001

   - Continents_Color_{1,2,3,4}: [from color set]
   _ Continents_Color_Overlay: [from color set]

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

/* high-temperature rocky planet notes:
   [uses giant_planet BP]

   - Scattering_Color: AA907B
   - Night_Color: 0.001 per channel
   - Sunset_Color_1: FFCD8C
   - Sunset_Color_2: FD9500

   - Equator_Clouds_Color_{Shift,Uniformity}: 0
   - Equator_Clouds_Coverage: 1-4 linear
   - Equator_Clouds_Shadows_{Size,Strength}: 0
   - Equator_Clouds_Color_{1,2,3,4}: [from color set]

   - {Tropics_Clouds_,Deep_Clouds_}*: Same as Equator_Clouds_*, except that
     Tropics_Clouds_Coverage is 1-=40 linear, and must be >= 10x
     Equator_Clouds_Coverage

   - Poles_Latitude: 0.4-0.7 linear
   - Poles_Clouds_Frequency: 0-0.6 linear
 */

// TODO: Put all the hardcoded numbers from this file into a Lua script.
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

    Ugame_user_settings * game_user_settings = Ugame_user_settings::get();
    check(game_user_settings);

    set_property(
        star_actor,
        TEXT("Atmospheric Phenomena"),
        2 < game_user_settings->planet_detail);
    set_property(star_actor, TEXT("Coronal Rays Count"), 128);
    set_property(star_actor, TEXT("Surface Explosions Count"), 8);
    set_property(star_actor, TEXT("Coronal Ejections Count"), 8);
    set_property(
        star_actor,
        TEXT("Activate Surface Rays"),
        2 < game_user_settings->planet_detail);

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

namespace {
    template<typename T, size_t Extent>
    T const & random_color_set(
        std::span<T const, Extent> all_sets, detail::rng_state & rng_state)
    {
        return all_sets[random_int(0, all_sets.size() - 1, rng_state)];
    }

    struct ring_colors
    {
        FLinearColor color_1;
        FLinearColor color_2;
        FLinearColor color_3;
        FLinearColor scattering_color;
    };

    // TODO: -> Lua script
    ring_colors const all_ring_colors[] = {
        // barren_planet BP
        ring_colors{
            FColor(0xB8, 0xC6, 0xE7),
            FColor(0xFF, 0xBF, 0xAB),
            FColor(0x04, 0x87, 0x9D),
            FColor(0xF3, 0xEF, 0xFF)},
        // ice_planet BP
        ring_colors{
            FColor(0xD9, 0xC0, 0xD4),
            FColor(0xBA, 0xC5, 0xE8),
            FColor(0xCB, 0xFF, 0xFD),
            FColor(0xFF, 0x9A, 0x94)},
        // rocky_planet BP (same as giant_planet BP)
        ring_colors{
            FColor(0x63, 0x96, 0xFF),
            FColor(0x6E, 0xD6, 0xF2),
            FColor(0xFF, 0x9E, 0x4B),
            FColor(0xFF, 0x9E, 0x4B)}};

    double
    rings_thickness(planet_t const & planet, detail::rng_state & rng_state)
    {
        std::normal_distribution dist{0.0, 0.25};
        double const roll = random_number(dist, rng_state);
        return std::min(roll * planet.mass_kg / earth_mass_kg, 2.5);
    }

    void configure_rings(
        AActor * planet_actor,
        planet_t const & planet,
        detail::rng_state & rng_state)
    {
        double const thickness = rings_thickness(planet, rng_state);
        if (thickness < 0.05)
            return;

        set_property(
            planet_actor,
            TEXT("Rings_Opacity"),
            random_double(1.0, 8.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Inner_Radius"),
            random_double(1.25, 2.0, rng_state));
        set_property(planet_actor, TEXT("Outer_Radius"), thickness);
        set_property(
            planet_actor,
            TEXT("Edge_Hardness"),
            random_double(0.75, 20.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Frequency"),
            random_double(0.75, 20.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Position"),
            random_double(0.0, 10.0, rng_state));

        double const desaturation = random_double(0.3, 0.5, rng_state);
        ring_colors const & colors =
            random_color_set(std::span(all_ring_colors), rng_state);

        set_property(
            planet_actor,
            TEXT("Rings_Color_1"),
            colors.color_1.Desaturate(desaturation));
        set_property(
            planet_actor,
            TEXT("Rings_Color_2"),
            colors.color_2.Desaturate(desaturation));
        set_property(
            planet_actor,
            TEXT("Rings_Color_3"),
            colors.color_3.Desaturate(desaturation));
        set_property(
            planet_actor,
            TEXT("Rings_Scattering_Color"),
            colors.scattering_color.Desaturate(desaturation));
    }

    struct high_temp_colors
    {
        FLinearColor scattering_color;
        FLinearColor sunset_color_1;
        FLinearColor sunset_color_2;
        FLinearColor clouds_color_1;
        FLinearColor clouds_color_2;
        FLinearColor clouds_color_3;
        FLinearColor clouds_color_4;
    };

    // TODO: -> Lua script
    high_temp_colors const all_high_temp_colors[] = {high_temp_colors{
        FColor(0xAA, 0x90, 0x7B),
        FColor(0xFF, 0xCD, 0x8C),
        FColor(0xFD, 0x95, 0x00),
        FColor(0xFF, 0xAC, 0x8C),
        FColor(0xFF, 0xBA, 0x8C),
        FColor(0x8F, 0x72, 0x4C),
        FColor(0xFF, 0xD2, 0x8C),
    }};

    struct giant_colors
    {
        FLinearColor scattering_color;
        FLinearColor sunset_color_1;
        FLinearColor sunset_color_2;

        FLinearColor equator_tropics_clouds_color_1;
        FLinearColor equator_tropics_clouds_color_2;
        FLinearColor equator_tropics_clouds_color_3;
        FLinearColor equator_tropics_clouds_color_4;

        FLinearColor deep_clouds_color_1;
        FLinearColor deep_clouds_color_2;
        FLinearColor deep_clouds_color_3;
        FLinearColor deep_clouds_color_4;

        FLinearColor poles_clouds_color_1;
        FLinearColor poles_clouds_color_2;
        FLinearColor poles_clouds_color_3;
        FLinearColor poles_clouds_color_4;
    };
}

namespace detail {
    // Playing games with operator overloading here so we can
    // Shift+Right-Click to copy colors out of the Unreal editor, and then
    // paste them here without modification.
    template<int N>
    struct color_channel_proxy;

    struct color_proxy
    {
        constexpr operator FLinearColor() const
        {
            return {colors_[0], colors_[1], colors_[2], colors_[3]};
        }

        template<int N>
        constexpr color_proxy operator,(color_channel_proxy<N> c)
        {
            color_proxy retval = *this;
            retval.colors_[N] = c.color_;
            return retval;
        }

        std::array<float, 4> colors_ = {};
    };

    template<int N>
    struct color_channel_proxy
    {
        float color_ = 0.0;

        constexpr color_proxy operator,(color_channel_proxy<1> c1)
            requires(N == 0)
        {
            color_proxy retval;
            retval.colors_[0] = color_;
            retval.colors_[1] = c1.color_;
            return retval;
        }
    };

    template<int N>
    struct color_proxy_tag
    {
        constexpr color_channel_proxy<N> operator=(double d)
        {
            return {float(d)};
        }
    };

    // For the syntax below to work, these must be globals.  To quiet truly
    // stupid MSVC warnings-as-errors enabled in Unreal builds, they must not
    // be in the global namespace.  So they live here in namespace detail.
    color_proxy_tag<0> R;
    color_proxy_tag<1> G;
    color_proxy_tag<2> B;
    color_proxy_tag<3> A;

    constexpr FLinearColor test_rgba = (R = 1.0, G = 2.0, B = 3.0, A = 4.0);
    static_assert(test_rgba.R == 1.0f);
    static_assert(test_rgba.G == 2.0f);
    static_assert(test_rgba.B == 3.0f);
    static_assert(test_rgba.A == 4.0f);

    // TODO: -> Lua script
    giant_colors const all_gas_giant_colors[] = {
        // defaults from Space Creator's BP_Gas_Planet_...
        giant_colors{
            .scattering_color =
                (R = 0.402777, G = 0.279992, B = 0.197445, A = 0.000000),
            .sunset_color_1 =
                (R = 0.260776, G = 0.429168, B = 1.000000, A = 1.000000),
            .sunset_color_2 =
                (R = 0.986111, G = 0.300067, B = 0.000000, A = 1.000000),

            .equator_tropics_clouds_color_1 =
                (R = 0.613108, G = 0.474955, B = 0.678819, A = 1.000000),
            .equator_tropics_clouds_color_2 =
                (R = 1.000000, G = 0.416886, B = 0.350444, A = 1.000000),
            .equator_tropics_clouds_color_3 =
                (R = 1.000000, G = 0.555718, B = 0.740559, A = 1.000000),
            .equator_tropics_clouds_color_4 =
                (R = 0.217057, G = 0.130974, B = 0.248264, A = 1.000000),

            .deep_clouds_color_1 =
                (R = 0.368034, G = 0.359555, B = 1.000000, A = 1.000000),
            .deep_clouds_color_2 =
                (R = 0.114281, G = 0.198232, B = 0.407986, A = 1.000000),
            .deep_clouds_color_3 =
                (R = 0.986836, G = 0.226007, B = 1.000000, A = 1.000000),
            .deep_clouds_color_4 =
                (R = 0.443815, G = 0.710699, B = 1.000000, A = 1.000000),

            .poles_clouds_color_1 =
                (R = 0.000000, G = 0.000000, B = 0.000000, A = 1.000000),
            .poles_clouds_color_2 =
                (R = 0.079554, G = 0.058569, B = 0.157986, A = 1.000000),
            .poles_clouds_color_3 =
                (R = 0.151064, G = 0.219453, B = 0.505208, A = 1.000000),
            .poles_clouds_color_4 =
                (R = 0.941485, G = 0.211042, B = 1.000000, A = 1.000000)},

        // More like Saturn (i.e. without the blueish colors above)
        giant_colors{
            .scattering_color =
                (R = 0.402777, G = 0.279992, B = 0.197445, A = 0.000000),
            .sunset_color_1 =
                (R = 0.104348, G = 0.056981, B = 0.003892, A = 1.000000),
            .sunset_color_2 =
                (R = 0.521739, G = 0.284905, B = 0.019459, A = 1.000000),

            .equator_tropics_clouds_color_1 =
                (R = 0.730461, G = 0.527115, B = 0.223228, A = 1.000000),
            .equator_tropics_clouds_color_2 =
                (R = 0.730461, G = 0.527115, B = 0.223228, A = 1.000000),
            .equator_tropics_clouds_color_3 =
                (R = 0.254152, G = 0.234551, B = 0.135633, A = 1.000000),
            .equator_tropics_clouds_color_4 =
                (R = 0.590619, G = 0.376262, B = 0.174647, A = 1.000000),

            .deep_clouds_color_1 =
                (R = 0.373913, G = 0.269823, B = 0.114267, A = 1.000000),
            .deep_clouds_color_2 =
                (R = 0.191304, G = 0.129079, B = 0.059495, A = 1.000000),
            .deep_clouds_color_3 =
                (R = 0.069565, G = 0.064200, B = 0.037125, A = 1.000000),
            .deep_clouds_color_4 =
                (R = 0.208696, G = 0.132952, B = 0.061712, A = 1.000000),

            .poles_clouds_color_1 =
                (R = 0.000000, G = 0.000000, B = 0.000000, A = 1.000000),
            .poles_clouds_color_2 =
                (R = 0.016026, G = 0.103665, B = 0.191304, A = 1.000000),
            .poles_clouds_color_3 =
                (R = 0.151064, G = 0.219453, B = 0.505208, A = 1.000000),
            .poles_clouds_color_4 =
                (R = 0.669565, G = 0.889855, B = 1.000000, A = 1.000000),
        }};

    // TODO: -> Lua script
    giant_colors const all_ice_giant_colors[] = {
        // colors from images of Neptune
        giant_colors{
            .scattering_color =
                (R = 0.059511, G = 0.158961, B = 0.930111, A = 1.000000),
            .sunset_color_1 =
                (R = 0.059511, G = 0.158961, B = 0.930111, A = 1.000000),
            .sunset_color_2 =
                (R = 0.124719, G = 0.205254, B = 0.478261, A = 1.000000),

            .equator_tropics_clouds_color_1 =
                (R = 0.082283, G = 0.223228, B = 0.982251, A = 1.000000),
            .equator_tropics_clouds_color_2 =
                (R = 0.070360, G = 0.162029, B = 1.000000, A = 1.000000),
            .equator_tropics_clouds_color_3 =
                (R = 0.048172, G = 0.097587, B = 0.623960, A = 1.000000),
            .equator_tropics_clouds_color_4 =
                (R = 0.254152, G = 0.658375, B = 1.000000, A = 1.000000),

            .deep_clouds_color_1 =
                (R = 0.016026, G = 0.043476, B = 0.191304, A = 1.000000),
            .deep_clouds_color_2 =
                (R = 0.070360, G = 0.162029, B = 1.000000, A = 1.000000),
            .deep_clouds_color_3 =
                (R = 0.048172, G = 0.097587, B = 0.623960, A = 1.000000),
            .deep_clouds_color_4 =
                (R = 0.669565, G = 0.848649, B = 1.000000, A = 1.000000),

            .poles_clouds_color_1 =
                (R = 0.000000, G = 0.000000, B = 0.000000, A = 1.000000),
            .poles_clouds_color_2 =
                (R = 0.016026, G = 0.043476, B = 0.191304, A = 1.000000),
            .poles_clouds_color_3 =
                (R = 0.070360, G = 0.162029, B = 1.000000, A = 1.000000),
            .poles_clouds_color_4 =
                (R = 0.070360, G = 0.162029, B = 1.000000, A = 1.000000)},

        // colors from images of Uranus
        giant_colors{
            .scattering_color =
                (R = 0.057805, G = 0.428690, B = 0.590619, A = 1.000000),
            .sunset_color_1 =
                (R = 0.260776, G = 0.876796, B = 1.000000, A = 1.000000),
            .sunset_color_2 =
                (R = 0.124719, G = 0.478261, B = 0.419337, A = 1.000000),


            .equator_tropics_clouds_color_1 =
                (R = 0.082283, G = 0.832256, B = 0.982251, A = 1.000000),
            .equator_tropics_clouds_color_2 =
                (R = 0.070360, G = 1.000000, B = 0.845060, A = 1.000000),
            .equator_tropics_clouds_color_3 =
                (R = 0.048172, G = 0.623960, B = 0.623960, A = 1.000000),
            .equator_tropics_clouds_color_4 =
                (R = 0.254152, G = 1.000000, B = 0.875692, A = 1.000000),

            .deep_clouds_color_1 =
                (R = 0.016026, G = 0.162091, B = 0.191304, A = 1.000000),
            .deep_clouds_color_2 =
                (R = 0.070360, G = 1.000000, B = 1.000000, A = 1.000000),
            .deep_clouds_color_3 =
                (R = 0.048172, G = 0.527995, B = 0.623960, A = 1.000000),
            .deep_clouds_color_4 =
                (R = 0.669565, G = 1.000000, B = 1.000000, A = 1.000000),

            .poles_clouds_color_1 =
                (R = 0.000000, G = 0.000000, B = 0.000000, A = 1.000000),
            .poles_clouds_color_2 =
                (R = 0.016026, G = 0.103665, B = 0.191304, A = 1.000000),
            .poles_clouds_color_3 =
                (R = 0.016026, G = 0.103665, B = 0.191304, A = 1.000000),
            .poles_clouds_color_4 =
                (R = 0.669565, G = 0.889855, B = 1.000000, A = 1.000000)}};
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
    // system_id=3 and planet_id=4) doesn't always looks the same, game
    // after game.
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

    configure_rings(planet_actor, planet, rng_state);

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

    configure_rings(planet_actor, planet, rng_state);

    set_property(planet_actor, TEXT("dirty"), true);
}

void configure_high_temperature_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    auto rng_state = detail::rng_state_from(planet.system_id, planet_id);

    high_temp_colors const & colors =
        random_color_set(std::span(all_high_temp_colors), rng_state);

    set_property(
        planet_actor, TEXT("Scattering Color"), colors.scattering_color);
    set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
    set_property(
        planet_actor, TEXT("Night Color"), FLinearColor(FVector(0.001)));
    set_property(planet_actor, TEXT("Sunset Color 1"), colors.sunset_color_1);
    set_property(planet_actor, TEXT("Sunset Color 2"), colors.sunset_color_2);

    double const equator_clouds_coverage = random_double(1.0, 4.0, rng_state);
    set_property(planet_actor, TEXT("Equator Clouds Color Shift"), 0.0);
    set_property(planet_actor, TEXT("Equator Clouds Color Uniformity"), 0.0);
    set_property(
        planet_actor, TEXT("Equator Clouds Coverage"), equator_clouds_coverage);
    set_property(planet_actor, TEXT("Equator Clouds Shadows Size"), 0.0);
    set_property(planet_actor, TEXT("Equator Clouds Shadows Strength"), 0.0);
    set_property(
        planet_actor, TEXT("Equator Clouds Color 1"), colors.clouds_color_1);
    set_property(
        planet_actor, TEXT("Equator Clouds Color 2"), colors.clouds_color_2);
    set_property(
        planet_actor, TEXT("Equator Clouds Color 3"), colors.clouds_color_3);
    set_property(
        planet_actor, TEXT("Equator Clouds Color 4"), colors.clouds_color_4);

    set_property(planet_actor, TEXT("Deep Clouds Color Shift"), 0.0);
    set_property(planet_actor, TEXT("Deep Clouds Color Uniformity"), 0.0);
    set_property(
        planet_actor, TEXT("Deep Clouds Coverage"), equator_clouds_coverage);
    set_property(planet_actor, TEXT("Deep Clouds Shadows Size"), 0.0);
    set_property(planet_actor, TEXT("Deep Clouds Shadows Strength"), 0.0);
    set_property(
        planet_actor, TEXT("Deep Clouds Color 1"), colors.clouds_color_1);
    set_property(
        planet_actor, TEXT("Deep Clouds Color 2"), colors.clouds_color_2);
    set_property(
        planet_actor, TEXT("Deep Clouds Color 3"), colors.clouds_color_3);
    set_property(
        planet_actor, TEXT("Deep Clouds Color 4"), colors.clouds_color_4);

    double const tropics_clouds_coverage = std::max(
        equator_clouds_coverage * 10.0, random_double(10.0, 40.0, rng_state));
    set_property(planet_actor, TEXT("Tropics Clouds Color Shift"), 0.0);
    set_property(planet_actor, TEXT("Tropics Clouds Color Uniformity"), 0.0);
    set_property(
        planet_actor, TEXT("Tropics Clouds Coverage"), tropics_clouds_coverage);
    set_property(planet_actor, TEXT("Tropics Clouds Shadows Size"), 0.0);
    set_property(planet_actor, TEXT("Tropics Clouds Shadows Strength"), 0.0);
    set_property(
        planet_actor, TEXT("Tropics Clouds Color 1"), colors.clouds_color_1);
    set_property(
        planet_actor, TEXT("Tropics Clouds Color 2"), colors.clouds_color_2);
    set_property(
        planet_actor, TEXT("Tropics Clouds Color 3"), colors.clouds_color_3);
    set_property(
        planet_actor, TEXT("Tropics Clouds Color 4"), colors.clouds_color_4);

    set_property(
        planet_actor,
        TEXT("Poles Latitude"),
        random_double(0.4, 0.7, rng_state));
    set_property(
        planet_actor,
        TEXT("Poles Clouds Frequency"),
        random_double(0, 0.6, rng_state));
    set_property(planet_actor, TEXT("Poles Color 1"), colors.clouds_color_1);
    set_property(planet_actor, TEXT("Poles Color 2"), colors.clouds_color_2);
    set_property(planet_actor, TEXT("Poles Color 3"), colors.clouds_color_3);
    set_property(planet_actor, TEXT("Poles Color 4"), colors.clouds_color_4);

    configure_rings(planet_actor, planet, rng_state);

    set_property(planet_actor, TEXT("dirty"), true);
}

namespace {
    void configure_giant_planet_impl(
        AActor * planet_actor,
        planet_t const & planet,
        detail::rng_state & rng_state,
        giant_colors const & colors)
    {
        check(planet_actor);

        set_property(
            planet_actor,
            TEXT("Global Tile Ratio"),
            random_double(0.1, 0.9, rng_state));
        set_property(
            planet_actor, TEXT("Scattering Color"), colors.scattering_color);
        set_property(planet_actor, TEXT("light_vector"), light_dir(planet));
        set_property(
            planet_actor, TEXT("Night Color"), FLinearColor(FVector(0.001)));
        set_property(
            planet_actor, TEXT("Sunset Color 1"), colors.sunset_color_1);
        set_property(
            planet_actor, TEXT("Sunset Color 2"), colors.sunset_color_2);

        double const equator_clouds_coverage =
            random_double(1.0, 3.0, rng_state);
        set_property(
            planet_actor,
            TEXT("Equator Clouds Color Shift"),
            random_double(0.0, 1.6, rng_state));
        set_property(
            planet_actor,
            TEXT("Equator Clouds Color Uniformity"),
            random_double(0.0, 4.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Equator Clouds Coverage"),
            equator_clouds_coverage);
        set_property(
            planet_actor,
            TEXT("Equator Clouds Shadows Size"),
            random_double(0.0, 0.07, rng_state));
        set_property(
            planet_actor,
            TEXT("Equator Clouds Shadows Strength"),
            random_double(0.0, 4.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Equator Clouds Color 1"),
            colors.equator_tropics_clouds_color_1);
        set_property(
            planet_actor,
            TEXT("Equator Clouds Color 2"),
            colors.equator_tropics_clouds_color_2);
        set_property(
            planet_actor,
            TEXT("Equator Clouds Color 3"),
            colors.equator_tropics_clouds_color_3);
        set_property(
            planet_actor,
            TEXT("Equator Clouds Color 4"),
            colors.equator_tropics_clouds_color_4);

        set_property(
            planet_actor,
            TEXT("Deep Clouds Color Shift"),
            random_double(0.0, 1.6, rng_state));
        set_property(
            planet_actor,
            TEXT("Deep Clouds Color Uniformity"),
            random_double(0.0, 4.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Deep Clouds Coverage"),
            equator_clouds_coverage);
        set_property(
            planet_actor,
            TEXT("Deep Clouds Shadows Size"),
            random_double(0.0, 0.07, rng_state));
        set_property(
            planet_actor,
            TEXT("Deep Clouds Shadows Strength"),
            random_double(0.0, 4.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Deep Clouds Color 1"),
            colors.deep_clouds_color_1);
        set_property(
            planet_actor,
            TEXT("Deep Clouds Color 2"),
            colors.deep_clouds_color_2);
        set_property(
            planet_actor,
            TEXT("Deep Clouds Color 3"),
            colors.deep_clouds_color_3);
        set_property(
            planet_actor,
            TEXT("Deep Clouds Color 4"),
            colors.deep_clouds_color_4);

        double const tropics_clouds_coverage = std::max(
            equator_clouds_coverage * 10.0,
            random_double(10.0, 30.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Color Shift"),
            random_double(0.0, 1.6, rng_state));
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Color Uniformity"),
            random_double(0.0, 4.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Coverage"),
            tropics_clouds_coverage);
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Shadows Size"),
            random_double(0.0, 0.07, rng_state));
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Shadows Strength"),
            random_double(0.0, 4.0, rng_state));
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Color 1"),
            colors.equator_tropics_clouds_color_1);
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Color 2"),
            colors.equator_tropics_clouds_color_2);
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Color 3"),
            colors.equator_tropics_clouds_color_3);
        set_property(
            planet_actor,
            TEXT("Tropics Clouds Color 4"),
            colors.equator_tropics_clouds_color_4);

        set_property(
            planet_actor,
            TEXT("Poles Latitude"),
            random_double(0.2, 0.5, rng_state));
        set_property(
            planet_actor,
            TEXT("Poles Clouds Frequency"),
            random_double(0, 0.6, rng_state));
        set_property(
            planet_actor, TEXT("Poles Color 1"), colors.poles_clouds_color_1);
        set_property(
            planet_actor, TEXT("Poles Color 2"), colors.poles_clouds_color_2);
        set_property(
            planet_actor, TEXT("Poles Color 3"), colors.poles_clouds_color_3);
        set_property(
            planet_actor, TEXT("Poles Color 4"), colors.poles_clouds_color_4);

        configure_rings(planet_actor, planet, rng_state);

        set_property(planet_actor, TEXT("dirty"), true);
    }
}

void configure_gas_giant_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    auto rng_state = detail::rng_state_from(planet.system_id, planet_id);

    giant_colors const & colors =
        random_color_set(std::span(detail::all_gas_giant_colors), rng_state);

    configure_giant_planet_impl(planet_actor, planet, rng_state, colors);
}

void configure_ice_giant_planet(
    AActor * planet_actor, planet_t const & planet, int planet_id)
{
    check(planet_actor);

    auto rng_state = detail::rng_state_from(planet.system_id, planet_id);

    giant_colors const & colors =
        random_color_set(std::span(detail::all_ice_giant_colors), rng_state);

    configure_giant_planet_impl(planet_actor, planet, rng_state, colors);
}
