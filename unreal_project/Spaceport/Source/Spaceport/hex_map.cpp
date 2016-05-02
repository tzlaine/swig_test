// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "utility.hpp"
#include "data_utility.hpp"
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

    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60.0f));
    float const national_border_thickness = 1.0f;
    float const province_border_thickness = 0.65f;
    float const indicator_move_time = 0.09f; // Should be <= the tick interval for Ahex_map.
    float const planet_star_z_scale = 0.2f;
    float const minor_planet_scale = 0.2f;
    float const major_planet_scale = 0.3f;
    float const star_scale = 0.5f;
    float const mb_scale = 0.125f;
    float const bats_scale = 0.125f;
	float const sb_scale = 0.125f;
	float const offmap_z = 0.25f;

    FVector hex_location (hex_coord_t hc, map_t const & map)
    {
        FVector retval;
        retval.X = hc.x * 1.5 * meters;
        retval.Y = (map.height - 1 - hc.y) * 2 * sin_60 * meters;
        if ((hc.x + 1000) % 2 == 1)
            retval.Y -= sin_60 * meters;
        return retval;
    }

}

Ahex_map::Ahex_map () :
    hover_indicator_mesh_ (nullptr),
    interior_hex_mesh_ (nullptr),
    edge_hex_mesh_ (nullptr),
    planet_mesh_ (nullptr),
    star5_mesh_ (nullptr),
    star6_mesh_ (nullptr),
    star8_mesh_ (nullptr),
    mobile_base_mesh_ (nullptr),
    battlestation_mesh_ (nullptr),
    starbase_mesh_ (nullptr),
    unit_square_mesh_ (nullptr),
    unit_cube_mesh_ (nullptr),
    solid_color_mat_ (nullptr),
    hex_border_mat_ (nullptr),
    hex_border_mesh_ (nullptr),
    thin_hex_border_mat_ (nullptr),
    player_controller_ (nullptr),
    hover_indicator_ (nullptr),
    hexes_instantiated_ (false),
    cursor_indicator_move_timeline_ (nullptr),
    unit_curve_ (nullptr),
    cursor_indicator_move_fn ()
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

    interior_hexes_.instances_.resize(primary_colors().size());
    edge_hexes_.instances_.resize(primary_colors().size());

    national_borders_.instances_.resize(primary_colors().size());
    province_borders_.instances_.resize(primary_colors().size());
    hex_borders_.instances_.resize(primary_colors().size());

    planets_.instances_.resize(primary_colors().size());
    star5s_.instances_.resize(primary_colors().size());
    star6s_.instances_.resize(primary_colors().size());
    star8s_.instances_.resize(primary_colors().size());

    mobile_bases_.instances_.resize(primary_colors().size());
    battlestations_.instances_.resize(primary_colors().size());
    starbases_.instances_.resize(primary_colors().size());

    offmap_panels_.instances_.resize(primary_colors().size());
    offmap_borders_.instances_.resize(primary_colors().size());

    for (auto const & pair : primary_colors()) {
        auto const nation_id = start_data_.nation(pair.first).nation_id;

        initialize(interior_hexes_, nation_id, pair.second, pair.first + "_interior_hexes");
        initialize(edge_hexes_, nation_id, pair.second, pair.first + "_edge_hexes");

        initialize(offmap_panels_, nation_id, pair.second, pair.first + "_offmap_panels");
    }

    for (auto const & pair : secondary_colors()) {
        auto const nation_id = start_data_.nation(pair.first).nation_id;

        initialize(national_borders_, nation_id, pair.second, pair.first + "_national_borders");
        initialize(province_borders_, nation_id, pair.second, pair.first + "_province_borders");
        initialize(hex_borders_, nation_id, pair.second, pair.first + "_hex_borders");

        initialize(planets_, nation_id, pair.second, pair.first + "_planets");
        initialize(star5s_, nation_id, pair.second, pair.first + "_star5s");
        initialize(star6s_, nation_id, pair.second, pair.first + "_star6s");
        initialize(star8s_, nation_id, pair.second, pair.first + "_star8s");

        initialize(mobile_bases_, nation_id, pair.second, pair.first + "_mobile_bases");
        initialize(battlestations_, nation_id, pair.second, pair.first + "_battlestations");
        initialize(starbases_, nation_id, pair.second, pair.first + "_starbases");

        initialize(offmap_borders_, nation_id, pair.second, pair.first + "_offmap_borders");
    }

    cursor_indicator_move_timeline_ =
        CreateDefaultSubobject<UTimelineComponent>("cursor_indicator_move_timeline_");
    cursor_indicator_move_timeline_->SetTimelineLength(1.0);
    cursor_indicator_move_timeline_->SetLooping(false);
    cursor_indicator_move_fn.BindUFunction(this, "cursor_indicator_move_callback");
}

void Ahex_map::cursor_indicator_move_callback (float x)
{
    float const alpha = FMath::Clamp(x / indicator_move_time, 0.0f, 1.0f);
    FVector const new_location = hover_indicator_from_ * (1.0f - alpha) + hover_indicator_to_ * alpha;
    hover_indicator_->SetWorldLocation(new_location);
}

void Ahex_map::BeginPlay ()
{
    Super::BeginPlay();
    cursor_indicator_move_timeline_->AddInterpFloat(unit_curve_, cursor_indicator_move_fn, "float");
    call_real_soon(instantiation_timer_, this, &Ahex_map::instantiate_hexes);
}

void Ahex_map::Tick (float delta_seconds)
{
    hex_coord_t const hc = hex_under_cursor();
    if (hc != invalid_hex_coord) {
        auto const old_location = hover_indicator_->GetComponentLocation();
        auto location = hex_location(hc, game_data_.map());
        location.Z = -0.05 * meters;

        if (!hover_indicator_->bVisible) {
            hover_indicator_->SetWorldLocation(location);
            hover_indicator_->SetVisibility(true);
        } else if (!old_location.Equals(location, 1.0f)) {
            hover_indicator_from_ = old_location;
            hover_indicator_to_ = location;
            cursor_indicator_move_timeline_->PlayFromStart();
        }

        auto const owner_id = owner(hc, start_data_, game_data_);
        auto const color = nation_id_secondary_colors_[owner_id];
        use_solid_color(hover_indicator_, color);
    } else {
        hover_indicator_->SetVisibility(false);
    }
}

hex_coord_t Ahex_map::hex_under_cursor () const
{
    hex_coord_t retval = invalid_hex_coord;

    if (!player_controller_ || !hexes_instantiated_)
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

void Ahex_map::initialize (instances_t & instances, int nation_id, FColor color, std::string const & name)
{
    auto & instanced = instances.by_color_[color];
    if (!instanced)
        instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(name.c_str());
    instanced->AttachTo(RootComponent);
    instances.instances_[nation_id] = instanced;
}

void Ahex_map::initialize_border_instanced_mesh (instances_t & instances, FColor color, float thickness, UMaterial * mat)
{
    auto instanced = instances.by_color_[color];
    UMaterialInstanceDynamic * material =
        instanced->CreateAndSetMaterialInstanceDynamicFromMaterial(0, mat);
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

void Ahex_map::instantiate_hexes ()
{
    if (!hover_indicator_mesh_ ||
        !interior_hex_mesh_ ||
        !edge_hex_mesh_ ||
        !planet_mesh_ ||
        !star5_mesh_ ||
        !star6_mesh_ ||
        !star8_mesh_ ||
        !mobile_base_mesh_ ||
        !battlestation_mesh_ ||
        !starbase_mesh_ ||
        !solid_color_mat_ ||
        !hex_border_mesh_ ||
        !hex_border_mat_ ||
        !thin_hex_border_mat_) {
        call_real_soon(instantiation_timer_, this, &Ahex_map::instantiate_hexes);
        return;
    }

    hover_indicator_->SetStaticMesh(hover_indicator_mesh_);
    hover_indicator_->SetVisibility(false);

    interior_hexes_.use(interior_hex_mesh_);
    edge_hexes_.use(edge_hex_mesh_);

    national_borders_.use(hex_border_mesh_);
    province_borders_.use(hex_border_mesh_);
    hex_borders_.use(hex_border_mesh_);

    planets_.use(planet_mesh_);
    star5s_.use(star5_mesh_);
    star6s_.use(star6_mesh_);
    star8s_.use(star8_mesh_);

    mobile_bases_.use(mobile_base_mesh_);
    battlestations_.use(battlestation_mesh_);
    starbases_.use(starbase_mesh_);

    // TODO: Probably this mapping should exist somewhere else as well.
    nation_id_primary_colors_.resize(primary_colors().size());
    for (auto const & pair : primary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_primary_colors_[nation_id] = pair.second;
        use_solid_color(interior_hexes_.instances_[nation_id], pair.second);
        use_solid_color(edge_hexes_.instances_[nation_id], pair.second);
    }

    nation_id_secondary_colors_.resize(secondary_colors().size());
    for (auto const & pair : secondary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        nation_id_secondary_colors_[nation_id] = pair.second;
        initialize_border_instanced_mesh(national_borders_, pair.second, national_border_thickness, hex_border_mat_);
        initialize_border_instanced_mesh(province_borders_, pair.second, province_border_thickness, hex_border_mat_);
        initialize_border_instanced_mesh(hex_borders_, pair.second, 1.0f, thin_hex_border_mat_);

        use_solid_color(planets_.instances_[nation_id], pair.second);
        use_solid_color(star5s_.instances_[nation_id], pair.second);
        use_solid_color(star6s_.instances_[nation_id], pair.second);
        use_solid_color(star8s_.instances_[nation_id], pair.second);

        use_solid_color(mobile_bases_.instances_[nation_id], pair.second);
        use_solid_color(battlestations_.instances_[nation_id], pair.second);
        use_solid_color(starbases_.instances_[nation_id], pair.second);
    }

    auto const & map = game_data_.map();
    for (int x = 0; x < map.width; ++x) {
        for (int y = 0; y < map.height; ++y) {
            hex_coord_t const hc = {x, y};
            instantiate_hex(hc);
        }
    }

    hexes_instantiated_ = true;
    call_real_soon(instantiation_timer_, this, &Ahex_map::create_offmap_areas);
}

void Ahex_map::create_offmap_areas ()
{
    if (!unit_square_mesh_ ||
        !unit_cube_mesh_ ||
        !hexes_instantiated_) {
        call_real_soon(instantiation_timer_, this, &Ahex_map::create_offmap_areas);
        return;
    }

    offmap_panels_.use(unit_square_mesh_);
    offmap_borders_.use(unit_cube_mesh_);

    for (auto const & pair : primary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        use_solid_color(offmap_panels_.instances_[nation_id], pair.second);
    }

    nation_id_secondary_colors_.resize(secondary_colors().size());
    for (auto const & pair : secondary_colors()) {
        auto nation_id = start_data_.nation(pair.first).nation_id;
        use_solid_color(offmap_borders_.instances_[nation_id], pair.second);
    }

    auto const & map = game_data_.map();

    std::vector<hex_coord_t> adjacent_hexes;
    for (auto const & pair : start_data_.map().starting_national_holdings) {
        auto const & nation = start_data_.nation(pair.first);
        auto const & offmap_area = pair.second.offmap_area;

        adjacent_hexes.resize(offmap_area.adjacent_hexes.size());
        std::transform(
            offmap_area.adjacent_hexes.begin(), offmap_area.adjacent_hexes.end(),
            adjacent_hexes.begin(),
            [](int hex_id) { return to_hex_coord(hex_id); }
        );
        std::sort(adjacent_hexes.begin(), adjacent_hexes.end());

        auto const first_location = hex_location(adjacent_hexes.front(), map);
        auto const last_location = hex_location(adjacent_hexes.back(), map);
        auto lower_left = first_location;
        auto upper_right = last_location;
        if (adjacent_hexes[0].x == 0 || adjacent_hexes[0].x == map.width - 1) {
            // vertical offmap area
			lower_left = last_location;
			upper_right = first_location;
			
			if (adjacent_hexes[0].x == 0)
                lower_left.X -= 2.25f * meters;
            else
                upper_right.X += 2.25f * meters;
            lower_left.Y -= sin_60 * meters;
            upper_right.Y += sin_60 * meters;
        } else {
            // horizontal offmap area
            if (adjacent_hexes[0].y == 0) {
                hex_coord_t const top_plus_one = { 0, -1 };
                auto const new_y = hex_location(top_plus_one, map).Y;
                upper_right.Y = new_y + sin_60 * meters;
            } else {
                hex_coord_t const bottom_minus_one = { 1, map.height };
                auto const new_y = hex_location(bottom_minus_one, map).Y;
                lower_left.Y = new_y - sin_60 * meters;
            }
            lower_left.X -= 0.5f * meters;
			upper_right.X += 0.5f * meters;
        }
        lower_left.Z = offmap_z * meters;
        upper_right.Z = offmap_z * meters;

        if (!offmap_area.features.empty()) {
			// TODO: For now, only SBs may appear as features....
			auto const hc = to_hex_coord(offmap_area.counter_hex);
            auto location = hex_location(hc, map);
            location.Z = offmap_z * meters;
            FTransform const transform(FRotator(0, 0, 0), location, FVector(sb_scale, sb_scale, planet_star_z_scale));
            starbases_.add(nation.nation_id, transform);
        }

		auto const position = (upper_right + lower_left) / 2.0f;
		auto const delta = upper_right - lower_left;
		FVector const scale(delta.X / 2.0f / meters, delta.Y / 2.0f / meters, 1.0);
		offmap_panels_.add(nation.nation_id, FTransform(FRotator(180, 0, 0), position, scale));
    }
}

void Ahex_map::instantiate_hex (hex_coord_t hc)
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

    // hex mesh
    if (x == 0 || y == 0 || x == map.width - 1 || y == map.height - 1)
        edge_hexes_.add(owner_id, FTransform(rotation, location));
    else
        interior_hexes_.add(owner_id, FTransform(rotation, location));

    // hex border meshes
    rotation.Roll = 180.0f;
    location.Z = 0 * meters;

    for (auto d : all_hex_directions) {
        auto other_hc = adjacent_hex_coord(hc, d);
        if (!on_map(other_hc, map)) {
            national_borders_.add(owner_id, FTransform(rotation, location));
        } else {
            auto const & other_hex = game_data_.hex(other_hc);
            auto const * other_province = game_data_.province(other_hex.province_id);
            auto const other_owner_id = owner(other_hc, start_data_, game_data_);
            if (other_owner_id != owner_id)
                national_borders_.add(owner_id, FTransform(rotation, location));
            else if (other_hex.province_id != map_hex.province_id)
                province_borders_.add(owner_id, FTransform(rotation, location));
            else
                hex_borders_.add(owner_id, FTransform(rotation, location));
        }
        rotation.Yaw += 60.0f;
    }

    // station, planet, and captial markers
    bool capital_hex = false;
    int star_points = 5;
    {
        auto const & nation = start_data_.nation(owner_id);
        auto const id = to_hex_id(map_hex.coord);
        capital_hex = std::count_if(
            nation.capital.hexes.begin(), nation.capital.hexes.end(),
            [=](start_data::capital_hex_t const & capital_hex) { return capital_hex.coord == id; }
        );
        star_points = nation.capital_star_points;
    }

    if (capital_hex) {
        FTransform const transform(rotation, location, FVector(star_scale, star_scale, planet_star_z_scale));
        switch (star_points) {
        default:
        case 5: star5s_.add(owner_id, transform); break;
        case 6: star6s_.add(owner_id, transform); break;
        case 8: star8s_.add(owner_id, transform); break;
        }
        return; // For capital hexes, don't show anything else.
    }

    bool planet_hex = false;
    planet_t::type_t planet_type = planet_t::type_t::minor;
    int sbs = 0;
    int batss = 0;
    int mbs = 0;
    for (auto const & zone : map_hex.zones) {
        for (auto const & fixture : zone.fixtures) {
            if (fixture.type == hex_zone_fixture_t::type_t::type_planet) {
                planet_hex = true;
                planet_type = fixture.planet.type;
            } else if (fixture.type == hex_zone_fixture_t::type_t::type_base) {
                if (is_sb(fixture.base))
                    ++sbs;
                else if (is_bats(fixture.base))
                    ++batss;
                else if (is_mb(fixture.base))
                    ++mbs;
            }
        }
    }

    if (planet_hex) {
        FTransform transform;
        if (planet_type == planet_t::type_t::minor)
            transform = FTransform(rotation, location, FVector(minor_planet_scale, minor_planet_scale, planet_star_z_scale));
        else
            transform = FTransform(rotation, location, FVector(major_planet_scale, major_planet_scale, planet_star_z_scale));
        // TODO: Adjust for the presence of sb, bats, or mb?
        planets_.add(owner_id, transform);
    } else if (0 < sbs) {
        FTransform const transform(rotation, location, FVector(sb_scale, sb_scale, planet_star_z_scale));
        // TODO: Adjust for the presence of other sb, bats, or mb?
        starbases_.add(owner_id, transform);
    } else if (0 < batss) {
        FTransform const transform(rotation, location, FVector(bats_scale, bats_scale, planet_star_z_scale));
        // TODO: Adjust for the presence of other sb, bats, or mb?
        battlestations_.add(owner_id, transform);
    } else if (0 < mbs) {
        FTransform const transform(rotation, location, FVector(mb_scale, mb_scale, planet_star_z_scale));
        // TODO: Adjust for the presence of other sb, bats, or mb?
        mobile_bases_.add(owner_id, transform);
    }
}

void Ahex_map::instances_t::use (UStaticMesh * mesh)
{
    for (auto pair : by_color_) {
        pair.second->SetStaticMesh(mesh);
    }
}

void Ahex_map::instances_t::add(int nation_id, FTransform transform)
{
    instances_[nation_id]->AddInstanceWorldSpace(transform);
}