#include "Ssave_load_dlg.h"
#include "game_instance.h"
#include "utility.hpp"
#include "widgets/Sstyled_text_block.h"
#include "widgets/Sstyled_button.h"
#include "widgets/Sstyled_editable_text_box.h"

#include <format>

#include <SlateOptMacros.h>
#include <Internationalization/Internationalization.h>
#include <Widgets/SCanvas.h>
#include <Widgets/SOverlay.h>
#include <Widgets/Layout/SScaleBox.h>
#include <Widgets/SUserWidget.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Layout/SConstraintCanvas.h>


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

/* Star notes:

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
     of planet generation.  Leave it at the lowest vaue unless the planet has
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

   - Atmosphere_Edge_Brightness: 0-1.0 should be 0.1 for most planets; arid
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

#include <boost/type_index.hpp>
#include "game_data.hpp"
#include "rng.hpp"
#include "text/beman_utf_view/utf_view.hpp"

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

struct loaded_textures
{
    UTexture * get(FString name)
    {
        if (textures_.Contains(name))
            return textures_[name];
        TSoftObjectPtr<UTexture> loader(name);
        UTexture * retval = loader.LoadSynchronous();
        if (!retval) {
            throw std::runtime_error(
                std::format("Could not get a pointer to texture {}", name));
        }
        return textures_[name] = retval;
    }

private:
    TMap<FString, UTexture *> textures_;
};

struct loaded_material_interfaces
{
    UMaterialInterface * get(FName name)
    {
        if (textures_.Contains(name))
            return textures_[name];
        FString material_path = FString::Printf(TEXT("Material'{}'"), name);
        ConstructorHelpers::FObjectFinder<UMaterial> finder(*material_path);
        if (!finder.Succeeded()) {
            throw std::runtime_error(
                std::format("Could not load base material {}", material_path));
        }
        return textures_[name] = finder.Object;
    }

private:
    TMap<FName, UMaterialInterface *> textures_;
};

// TODO: Aplanet_actor?
void configure_map_star(
    AStaticMeshActor * star_actor,
    system_t const & system,
    star_t const & star,
    loaded_textures & textures,
    loaded_material_interfaces & material_interfaces)
{
    check(star_class_t::invalid_star_class < star.star_class);
    check(star.star_class <= star_class_t::m);

    star_actor->SetActorLocation(FVector(system.world_pos_x, system.world_pos_y, 0));

    FName texture_name;
    switch (star.star_class) {
    case star_class_t::o:
        texture_name =
            TEXT("/Game/levels/star_materials/blue_map_star.blue_map_star");
        break;
    case star_class_t::b:
    case star_class_t::a:
        texture_name = TEXT(
            "/Game/levels/star_materials/"
            "blue_white_map_star.blue_white_map_star");
        break;
    case star_class_t::f:
        texture_name =
            TEXT("/Game/levels/star_materials/white_map_star.white_map_star");
        break;
    case star_class_t::g:
    case star_class_t::k: {
        texture_name = *FString::Printf(
            TEXT("/Game/levels/star_materials/"
                 "yellow_map_star_{}.yellow_map_star_{}"),
            random_int(0, 1));
        break;
    }
    case star_class_t::m: {
        texture_name = *FString::Printf(
            TEXT("/Game/levels/star_materials/red_map_star_{}.red_map_star_{}"),
            random_int(0, 2));
        break;
    }
    }

    UMaterialInterface * base_material = material_interfaces.get(texture_name);
    UMaterialInstanceDynamic * instance =
        UMaterialInstanceDynamic::Create(base_material, star_actor);

    // https://en.wikipedia.org/wiki/Stellar_classification

    {
        // Map this star's luminosity relative to other main sequece stars (in
        // log space) into the (linear) range of visual intensity values.
        double const log_min_lum = std::log10(0.08);
        double const log_max_lum = std::log10(30000.0);
        double const log_lum = std::log10(star.solar_luminosities);
        double const alpha =
            (log_lum - log_min_lum) / (log_max_lum - log_min_lum);
        instance->SetScalarParameterValue(
            TEXT("Burst_Intensity"), std::lerp(0.5, 40.0, alpha));
    }

    // Class A and brighter are >= 5x the sun.
    bool const use_wide_lense_flare = 5.0 < star.solar_luminosities;

    UTexture * texture = nullptr;
    if (use_wide_lense_flare) {
        int i = random_int(0, 4);
        if (i ==0)
            i = 7;
        FString path = FString::Printf(TEXT("T_LensFlare_{}"), i);
        texture = textures.get(path);
    } else {
        FString path =
            FString::Printf(TEXT("T_LensFlare_{}"), random_int(5, 6));
        texture = textures.get(path);
    }

    instance->SetTextureParameterValue(TEXT("Texture_Main_Flare"), texture);
    instance->SetScalarParameterValue(
        TEXT("Halo_Size"),
        use_wide_lense_flare ? random_double(0.1, 0.4)
                             : random_double(0.1, 0.25));

    star_actor->GetStaticMeshComponent()->SetMaterial(0, instance);
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

// TODO: Aplanet_actor?
// TODO: For very cold planets that get terraformed, use the BP_Planet_Ice
// blueprint from Space_Creator, and reduce the ice over time, as the planet
// is terraformed.  After the terraforming has gotten close enough to
// Earthlike conditions common, change it to use the BP_Planet_Terran
// blueprint, keeping the properties that they have in common.
void configure_habitable_planet(
    AActor * planet_actor, planet_t const & planet, loaded_textures & textures)
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

    // TODO: Need a graphics setting for shader complexity.
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

    FString mountains_tex_name = FString::Printf(
        TEXT("/Game/Space_Creator/PlanetCreator_1_V2/Textures/Color_Textures/"
             "T_PlanetTexture_Color_{}.T_PlanetTexture_Color_{}"),
        1);
    UTexture * mountains_tex = textures.get(mountains_tex_name);
    set_property(planet_actor, TEXT("T_Mountains"), mountains_tex);

    FString plains_tex_name = FString::Printf(
        TEXT("/Game/Space_Creator/PlanetCreator_1_V2/Textures/Color_Textures/"
             "T_PlanetTexture_Color_{}.T_PlanetTexture_Color_{}"),
        2);
    UTexture * plains_tex = textures.get(plains_tex_name);
    set_property(planet_actor, TEXT("T_Plains"), plains_tex);

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
        std::lerp(0.4, 0.6, 1.0 - temperature_alpha));
    set_property(
        planet_actor,
        TEXT("Ice_Coverage"),
        std::lerp(0.1, 0.5, 1.0 - temperature_alpha));

    // clouds
    double clouds_speed = 0.0001;
    if (double alpha = seasons_intensity_factor(planet))
        clouds_speed = std::lerp(0.0001, 0.005, alpha);
    set_property(planet_actor, TEXT("Clouds_Speed"), clouds_speed);

    set_property(planet_actor, TEXT("Clouds_Opacity"), random_double(2.0, 4.0));
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
    set_property(planet_actor, TEXT("Atmosphere_Direct_Brightness"), 0.1);
    set_property(planet_actor, TEXT("Atmosphere_Edge_Brightness"), 0.1);
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

namespace {
    TSharedRef<ITableRow> make_row(
        Ssave_load_dlg::item_data data,
        TSharedRef<STableViewBase> const & table)
    {
        return SNew(STableRow<TSharedPtr<FString>>, table)
            .Padding(2.0f)[SNew(Sstyled_text_block)
                           .Text(FText::FromString(*data))];
    }

    std::filesystem::path
    default_filename(std::string nation, std::chrono::utc_clock::time_point t)
    {
        return std::format("{}-{:%F}", nation, t);
    }
}

void Ssave_load_dlg::Construct(FArguments const & args)
{
    UFont * title_font = detail::stream_default_font();

    saving_ = args._saving;
    in_game_ = args._in_game;

    FText action_text =
        loc_text(saving_ ? TEXT("save_game") : TEXT("load_game"));

    auto do_action = [this] {
        if (saving_) {
            if (filename_is_in_list()) {
                playing_hud()->do_after_confirming(
                    [this] {
                        if (auto * pc = player_controller())
                            pc->server_save_game(filename_);
                        hide();
                    },
                    TEXT("confirm_save_over"),
                    TEXT("previous_save_will_be_lost"),
                    TEXT("save_over"));
            } else {
                if (auto * pc = player_controller())
                    pc->server_save_game(filename_);
                hide();
            }
        } else if (in_game_) {
            playing_hud()->do_after_confirming(
                [this] {
                    if (auto * pc = player_controller_base())
                        pc->server_load_game(filename_);
                },
                TEXT("confirm_load"),
                TEXT("unsaved_progress_lost"),
                TEXT("load_game"));
        } else {
            if (auto * pc = player_controller_base())
                pc->server_load_game(filename_);
        }
        return FReply::Handled();
    };

    // TODO: Allow selecting one or more saves, and deleting them.

    // clang-format off
    ChildSlot[
        SNew(SConstraintCanvas)

        +SConstraintCanvas::Slot().Anchors(FAnchors(0, 0, 1, 1))[
            SNew(SImage).ColorAndOpacity(FSlateColor(FColor(0, 0, 0, 200)))]

        +SConstraintCanvas::Slot().Anchors(FAnchors(0.4, 0.2, 0.6, 0.8))[
            SNew(SBorder).Padding(50.0f)[ // TODO: Use a styled one.

                SNew(SVerticalBox)
                +SVerticalBox::Slot().FillHeight(5)

                +SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)[
                    SNew(Sstyled_text_block)
                    .Text(action_text)
                    .Font(FSlateFontInfo(title_font,
                                         ui_defaults().title_font_size_))]

                +SVerticalBox::Slot().FillHeight(5)

                +SVerticalBox::Slot().FillHeight(15).HAlign(HAlign_Fill)[
                    SAssignNew(edit_, Sstyled_editable_text_box)
                    .OnTextCommitted_Lambda(
                        [do_action, this](FText const & text, ETextCommit::Type type) {
                            if (type != ETextCommit::OnEnter)
                                return;
                            filename_ = text.ToString();
                            do_action();
                        })
                    .OnTextChanged_Lambda([this](FText const & text) {
                        filename_ = text.ToString();
                        action_button_->SetEnabled(filename_is_well_formed());
                    })
                ]

                +SVerticalBox::Slot().FillHeight(70).HAlign(HAlign_Fill)[
                    SAssignNew(list_box_, Sstyled_list_view<item_data>)
                    .ItemHeight(ui_defaults().font_size_)
                    .ListItemsSource(&items_)
                    .OnGenerateRow_Lambda(&make_row)
                    .SelectionMode(ESelectionMode::Single)
                    .OnMouseButtonDoubleClick_Lambda(
                        [do_action, this](Ssave_load_dlg::item_data data) {
                            if (!data)
                                return;
                            filename_ = *data;
                            do_action();
                        })
                    .OnSelectionChanged_Lambda(
                        [this](Ssave_load_dlg::item_data data,
                               ESelectInfo::Type) {
                            if (data)
                                filename_ = *data;
                            else
                                filename_.Reset();
                            edit_->SetText(FText::FromString(filename_));
                            action_button_->SetEnabled(filename_is_well_formed());
                        })]

                +SVerticalBox::Slot().FillHeight(5)

                +SVerticalBox::Slot().AutoHeight()[
                    SNew(SHorizontalBox)

                    +SHorizontalBox::Slot().FillWidth(1)

                    +SHorizontalBox::Slot().AutoWidth()[
                        SAssignNew(action_button_, Sstyled_button).Text(action_text)
                        .OnClicked_Lambda(do_action)]

                    +SHorizontalBox::Slot().FillWidth(0.5)

                    +SHorizontalBox::Slot().AutoWidth()[
                        SNew(Sstyled_button).Text(loc_text(TEXT("cancel")))
                        .OnClicked_Lambda([this] {
                            hide();
                            return FReply::Handled();
                        })]

                    +SHorizontalBox::Slot().FillWidth(1)]

                +SVerticalBox::Slot().FillHeight(2)]]];
    // clang-format on

    if (saving_) {
        FString s = to_fstring(default_filename("TODO", std::chrono::utc_clock::now())) ;
        edit_->SetText(FText::FromString(s));
    }

    action_button_->SetEnabled(filename_is_well_formed());
}

void Ssave_load_dlg::saves_changed(TArray<FString> const & files)
{
    items_.SetNum(files.Num());
    std::transform(begin(files), end(files), begin(items_), [](auto const & e) {
        return item_data(new FString(e));
    });
    if (list_box_)
        list_box_->RequestListRefresh();
}

bool Ssave_load_dlg::cancelable() { return true; }

bool Ssave_load_dlg::filename_is_well_formed() const
{
    if (saving_)
        return !filename_.IsEmpty();
    return filename_is_in_list();
}

bool Ssave_load_dlg::filename_is_in_list() const
{
    return std::any_of(begin(items_), end(items_), [this](auto const & e) {
        return e && *e == filename_;
    });
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
