// Fill out your copyright notice in the Description page of Project Settings.

#include "Spaceport.h"
#include "hex_map.h"
#include "utility.hpp"
#include "data_utility.hpp"
#include "hex_operations.hpp"

#if 0
#include "SlateStats.h"
#include "Stats.h"
#include "widgets/styleable_button.h"
#include "widgets/styleable_text_block.h"
#include "widgets/styleable_text_block.h"
#endif

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

    float const sin_60 = FMath::Sin(FMath::DegreesToRadians(60.0f));
    float const indicator_move_time = 0.09f; // Should be <= the tick interval for Ahex_map.

    FVector hex_location (hex_coord_t hc, map_t const & map)
    {
        FVector retval;
        retval.X = hc.x * 1.5 * meters;
        retval.Y = (map.height - 1 - hc.y) * 2 * sin_60 * meters;
        if ((hc.x + 1000) % 2 == 1)
            retval.Y -= sin_60 * meters;
        return retval;
    }

    FColor to_fcolor (visual_config::color_t color)
    { return FColor(color.r, color.g, color.b); }

}

Ahex_map::Ahex_map () :
    hover_indicator_mesh_ (nullptr),
    large_fleet_actor_ (nullptr),
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
    text_mat_ (nullptr),
    hex_border_mat_ (nullptr),
    hex_border_mesh_ (nullptr),
    thin_hex_border_mat_ (nullptr),
    player_controller_ (nullptr),
    hover_indicator_ (nullptr),
    hexes_instantiated_ (false),
    cursor_indicator_move_timeline_ (nullptr),
    unit_curve_ (nullptr),
    cursor_indicator_move_fn ()
    ,showing_ui_(false)
{
    PrimaryActorTick.bCanEverTick = true;

    // TODO: Move the start-data loading code to a more appropriate location.
    start_data_.init_nations(load_text_file("nations.json"));
    start_data_.init_unit_defs(load_text_file("units.json"));
    auto load = [](const std::string & filename) { return load_text_file(filename); };
    start_data_.init_scenario(load_text_file("scenarios/the_wind.json"), load, load);
    game_data_ = game_data_t(start_data_);

    // TODO: Move this somewhere else?
    visual_config_.init_hex_map(load_text_file("hex_map_config.json"), start_data_);

    hover_indicator_ = CreateDefaultSubobject<UStaticMeshComponent>("hover_indicator");
    hover_indicator_->AttachTo(RootComponent);

    auto const num_primary_colors = visual_config_.hex_map().primary_colors.size();

    interior_hexes_.instances_.resize(num_primary_colors);
    edge_hexes_.instances_.resize(num_primary_colors);

    national_borders_.instances_.resize(num_primary_colors);
    province_borders_.instances_.resize(num_primary_colors);
    hex_borders_.instances_.resize(num_primary_colors);

    planets_.instances_.resize(num_primary_colors);
    star5s_.instances_.resize(num_primary_colors);
    star6s_.instances_.resize(num_primary_colors);
    star8s_.instances_.resize(num_primary_colors);

    mobile_bases_.instances_.resize(num_primary_colors);
    battlestations_.instances_.resize(num_primary_colors);
    starbases_.instances_.resize(num_primary_colors);

    offmap_panels_.instances_.resize(num_primary_colors);
    offmap_borders_.instances_.resize(num_primary_colors);

    for (auto const & pair : visual_config_.hex_map().primary_colors) {
        auto const nation_id = start_data_.nation_id(pair.first);
        FColor const color = to_fcolor(pair.second);

        initialize(interior_hexes_, nation_id, color, make_string(pair.first, "_interior_hexes"));
        initialize(edge_hexes_, nation_id, color, make_string(pair.first, "_edge_hexes"));

        initialize(offmap_panels_, nation_id, color, make_string(pair.first, "_offmap_panels"));
    }

    for (auto const & pair : visual_config_.hex_map().secondary_colors) {
        auto const nation_id = start_data_.nation_id(pair.first);
        FColor const color = to_fcolor(pair.second);

        initialize(national_borders_, nation_id, color, make_string(pair.first, "_national_borders"));
        initialize(province_borders_, nation_id, color, make_string(pair.first, "_province_borders"));
        initialize(hex_borders_, nation_id, color, make_string(pair.first, "_hex_borders"));

        initialize(planets_, nation_id, color, make_string(pair.first, "_planets"));
        initialize(star5s_, nation_id, color, make_string(pair.first, "_star5s"));
        initialize(star6s_, nation_id, color, make_string(pair.first, "_star6s"));
        initialize(star8s_, nation_id, color, make_string(pair.first, "_star8s"));

        initialize(mobile_bases_, nation_id, color, make_string(pair.first, "_mobile_bases"));
        initialize(battlestations_, nation_id, color, make_string(pair.first, "_battlestations"));
        initialize(starbases_, nation_id, color, make_string(pair.first, "_starbases"));

        initialize(offmap_borders_, nation_id, color, make_string(pair.first, "_offmap_borders"));
    }

    for (auto const & pair : start_data_.map().starting_national_holdings) {
        auto const & nation = start_data_.nation(pair.first);
        auto const & offmap_area = pair.second.offmap_area;

        if (offmap_area.adjacent_hexes.empty())
            continue;

        auto text_render_component =
            CreateDefaultSubobject<UTextRenderComponent>(make_string(pair.first, "_offmap_label").c_str());
        offmap_labels_[nation.nation_id] = text_render_component;
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
    if (!showing_ui_) {
        std::string const sheet =
            "sheet alert_dialog\n"
            "{\n"
            "output:\n"
            "    result <== { dummy_value: 42 };\n"
            "}\n";
        std::string const layout_whole =
            "layout alert_dialog\n"
            "{\n"
            "    view dialog(name: \"Alert\", placement: place_row)\n"
            "    {\n"
            "        image(image: \"stop.tga\");\n"
            "\n"
            "        column(vertical: align_fill)\n"
            "        {\n"
            "            static_text(name: \"Unfortunately, something drastic has happened. If you would like we can try to continue with the operation, but there is a chance you will blow up your computer. Would you like to try?\", characters: 25);\n"
            "            row(vertical: align_bottom, horizontal: align_right)\n"
            "            {\n"
            "                button(name: \"Cancel\", action: @cancel, cancel: true);\n"
            "                button(name: \"OK\", bind: @result, action: @ok, default: true);\n"
            "            }\n"
            "        }\n"
            "    }\n"
            "}\n";
        std::string const layout =
            "layout alert_dialog\n"
            "{\n"
            "    view dialog(name: \"Alert\", placement: place_row)\n"
            "    {\n"
            "        column(vertical: align_fill)\n"
            "        {\n"
            "            static_text(name: \"Unfortunately, something drastic has happened. If you would like we can try to continue with the operation, but there is a chance you will blow up your computer. Would you like to try?\", characters: 25);\n"
            "            row(vertical: align_bottom, horizontal: align_right)\n"
            "            {\n"
            "                checkbox(name: \"Check me\");\n"
            "                button(name: \"Cancel\", action: @cancel, cancel: true);\n"
            "                button(name: \"OK\", bind: @result, action: @ok, default: true);\n"
            "            }\n"
            "        }\n"
            "    }\n"
            "}\n";
        std::istringstream sheet_stream(sheet);
        std::istringstream layout_stream(layout);
        adam_eve_ui_ = adam_eve_ui_t(GetWorld(), layout_stream, "inline layout", sheet_stream, "inline sheet");
        auto & root_widget = adam_eve_ui_.root_widget();
        root_widget.AddToViewport();
#if 0
#if 0
        //auto const & slate_app = FSlateApplication::Get();

        TSharedRef<SWindow> SlateWin = SNew(SWindow)
            .AutoCenter(EAutoCenter::None)
            .Title(FText::FromString(TEXT("Control Window")))
            .IsInitiallyMaximized(false)
            .ScreenPosition(FVector2D(100, 100))
            .ClientSize(FVector2D(500, 800))
            .CreateTitleBar(true)
            .SizingRule(ESizingRule::UserSized)
            .SupportsMaximize(false)
            .SupportsMinimize(true)
            .HasCloseButton(true);

        //TSharedRef<SWindow> SlateWinRef = SlateWin.ToSharedRef();

        FSlateApplication & SlateApp = FSlateApplication::Get();

        SlateApp.AddWindow(SlateWin, true);

        //SlateWinRef->SetContent(SNew(SControlWidget));
#endif

        Uroot_widget * root_widget = CreateWidget<Uroot_widget>(GetWorld()->GetFirstPlayerController(), Uroot_widget::StaticClass());
        {
            auto widget_and_slot = root_widget->new_child<Ustyleable_button>();
            auto slot = widget_and_slot.slot_;
            slot->SetAutoSize(true);
        }
        {
            auto widget_and_slot = root_widget->new_child<Ustyleable_text_block>();
            widget_and_slot.widget_->SetText(FText::FromString(TEXT("Some text!")));
            auto slot = widget_and_slot.slot_;
            slot->SetAutoSize(true);
        }
        root_widget->AddToViewport();

#if 0 // This is the run-modal code.
        auto & slate_app = FSlateApplication::Get();
        slate_app.GetRenderer()->FlushCommands();

        // Show the cursor if it was previously hidden so users can interact with the window
#if 0
        if ( slate_app.PlatformApplication->Cursor.IsValid() )
        {
            slate_app.PlatformApplication->Cursor->Show( true );
        }
#endif

        //Throttle loop data
        float LastLoopTime = (float)FPlatformTime::Seconds();
        const float MinThrottlePeriod = (1.0f / 60.0f); //Throttle the loop to a maximum of 60Hz

                                                        // Tick slate from here in the event that we should not return until the modal window is closed.
        while( true/*InSlateWindow == GetActiveModalWindow()*/ )
        {
            //Throttle the loop
            const float CurrentLoopTime = FPlatformTime::Seconds();
            const float SleepTime = MinThrottlePeriod - (CurrentLoopTime-LastLoopTime);
            LastLoopTime = CurrentLoopTime;
            if (SleepTime > 0.0f)
            {
                // Sleep a bit to not eat up all CPU time
                FPlatformProcess::Sleep(SleepTime);
            }

            FPlatformMisc::BeginNamedEvent(FColor::Magenta, "Slate::Tick");

            {
#if 0
                SCOPE_CYCLE_COUNTER(STAT_SlateTickTime);
                SLATE_CYCLE_COUNTER_SCOPE(GSlateTotalTickTime);
#endif

                const float DeltaTime = slate_app.GetDeltaTime();

                // Tick and pump messages for the platform.
                slate_app.TickPlatform_(DeltaTime);

                // It's possible that during ticking the platform we'll find out the modal dialog was closed.
                // in which case we need to abort the current flow.
                if ( false/*InSlateWindow != GetActiveModalWindow()*/ )
                {
                    break;
                }

                // Tick and render Slate
                slate_app.TickApplication_(DeltaTime);
            }

            // Update Slate Stats
            SLATE_STATS_END_FRAME(GetCurrentTime());

            FPlatformMisc::EndNamedEvent();

            // Synchronize the game thread and the render thread so that the render thread doesn't get too far behind.
            slate_app.GetRenderer()->Sync();
        }
#endif
#endif

        showing_ui_ = true;
    }
    hex_coord_t const hc = hex_under_cursor();
    if (hc != invalid_hex_coord) {
        // TODO: Offmap area hit test.
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

void Ahex_map::initialize_border_instanced_mesh (
    instances_t & instances,
    FColor color,
    float thickness,
    UMaterial * mat
) {
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

    auto const & hex_map_config = visual_config_.hex_map();

    // TODO: Probably this mapping should exist somewhere else as well.
    nation_id_primary_colors_.resize(hex_map_config.primary_colors.size());
    for (auto const & pair : hex_map_config.primary_colors) {
        auto nation_id = start_data_.nation_id(pair.first);
        FColor const color = to_fcolor(pair.second);
        nation_id_primary_colors_[nation_id] = color;
        use_solid_color(interior_hexes_.instances_[nation_id], color);
        use_solid_color(edge_hexes_.instances_[nation_id], color);
    }

    nation_id_secondary_colors_.resize(hex_map_config.secondary_colors.size());
    for (auto const & pair : hex_map_config.secondary_colors) {
        auto nation_id = start_data_.nation_id(pair.first);
        FColor const color = to_fcolor(pair.second);
        nation_id_secondary_colors_[nation_id] = color;
        initialize_border_instanced_mesh(
            national_borders_,
            color,
            hex_map_config.national_border_thickness,
            hex_border_mat_
        );
        initialize_border_instanced_mesh(
            province_borders_,
            color,
            hex_map_config.province_border_thickness,
            hex_border_mat_
        );
        initialize_border_instanced_mesh(hex_borders_, color, 1.0f, thin_hex_border_mat_);

        use_solid_color(planets_.instances_[nation_id], color);
        use_solid_color(star5s_.instances_[nation_id], color);
        use_solid_color(star6s_.instances_[nation_id], color);
        use_solid_color(star8s_.instances_[nation_id], color);

        use_solid_color(mobile_bases_.instances_[nation_id], color);
        use_solid_color(battlestations_.instances_[nation_id], color);
        use_solid_color(starbases_.instances_[nation_id], color);
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
        !text_mat_ ||
        !hexes_instantiated_) {
        call_real_soon(instantiation_timer_, this, &Ahex_map::create_offmap_areas);
        return;
    }

    offmap_panels_.use(unit_square_mesh_);
    offmap_borders_.use(unit_cube_mesh_);

    auto const & hex_map_config = visual_config_.hex_map();

    for (auto const & pair : hex_map_config.primary_colors) {
        auto nation_id = start_data_.nation_id(pair.first);
        FColor const color = to_fcolor(pair.second);
        use_solid_color(offmap_panels_.instances_[nation_id], color);
    }

    nation_id_secondary_colors_.resize(hex_map_config.secondary_colors.size());
    for (auto const & pair : hex_map_config.secondary_colors) {
        auto nation_id = start_data_.nation_id(pair.first);
        FColor const color = to_fcolor(pair.second);
        use_solid_color(offmap_borders_.instances_[nation_id], color);
    }

    auto const & map = game_data_.map();

    float const offmap_left_right_gap = (hex_map_config.offmap_left_right_thickness - 1.5) * meters;
    float const offmap_top_bottom_gap = (hex_map_config.offmap_top_bottom_thickness + sin_60) * meters;

    std::vector<hex_coord_t> adjacent_hexes;
    for (auto const & pair : start_data_.map().starting_national_holdings) {
        auto const & nation = start_data_.nation(pair.first);
        auto const & offmap_area = pair.second.offmap_area;

        if (offmap_area.adjacent_hexes.empty())
            continue;

        adjacent_hexes.resize(offmap_area.adjacent_hexes.size());
        std::transform(
            offmap_area.adjacent_hexes.begin(), offmap_area.adjacent_hexes.end(),
            adjacent_hexes.begin(),
            [](int id) { return hex_id_t(id).to_hex_coord(); }
        );
        std::sort(adjacent_hexes.begin(), adjacent_hexes.end());

        auto const left =
            adjacent_hexes.front().x == 0 && adjacent_hexes.back().x == 0;
        auto const right =
            adjacent_hexes.front().x == map.width - 1 && adjacent_hexes.back().x == map.width - 1;
        auto const top =
            adjacent_hexes.front().y == 0 && adjacent_hexes.back().y == 0;
        auto const bottom =
            adjacent_hexes.front().y == map.height - 1 && adjacent_hexes.back().y == map.height - 1;

        auto const first_location = hex_location(adjacent_hexes.front(), map);
        auto const last_location = hex_location(adjacent_hexes.back(), map);
        auto lower_left = first_location;
        auto upper_right = last_location;
        if (left || right) {
            // vertical offmap area
            lower_left = last_location;
            upper_right = first_location;

            if (left)
                lower_left.X -= hex_map_config.offmap_left_right_thickness * meters;
            else
                upper_right.X += hex_map_config.offmap_left_right_thickness * meters;
            lower_left.Y -= sin_60 * meters;
            upper_right.Y += sin_60 * meters;
        } else {
            // horizontal offmap area
            if (top) {
                hex_coord_t const top_plus_one = { 0, -1 };
                auto const new_y = hex_location(top_plus_one, map).Y;
                upper_right.Y = new_y + hex_map_config.offmap_top_bottom_thickness * meters;
            } else {
                hex_coord_t const bottom_minus_one = { 1, map.height };
                auto const new_y = hex_location(bottom_minus_one, map).Y;
                lower_left.Y = new_y - hex_map_config.offmap_top_bottom_thickness * meters;
            }
            lower_left.X -= 0.5f * meters;
            upper_right.X += 0.5f * meters;
        }
        lower_left.Z = hex_map_config.offmap_z * meters;
        upper_right.Z = hex_map_config.offmap_z * meters;

        auto feature_hex_it = offmap_area.feature_hexes.begin();
        for (auto const feature : offmap_area.features) {
            // TODO: For now, only SBs may appear as features....
            auto const hc = hex_id_t(*feature_hex_it++).to_hex_coord();
            auto location = hex_location(hc, map);
            location.Z = hex_map_config.offmap_z * meters;
            FTransform const transform(
                FRotator(0, 0, 0),
                location,
                FVector(
                    hex_map_config.sb_scale,
                    hex_map_config.sb_scale,
                    hex_map_config.planet_star_thickness
                )
            );
            starbases_.add(nation.nation_id, transform);
        }

        auto const panel_position = (upper_right + lower_left) / 2.0f;
        auto const panel_delta = upper_right - lower_left;
        FVector const panel_scale(panel_delta.X / 2.0f / meters, panel_delta.Y / 2.0f / meters, 1.0);
        offmap_panels_.add(nation.nation_id, FTransform(FRotator(180, 0, 0), panel_position, panel_scale));

        if (left || right) {
            auto side_bar_position = panel_position;
            if (left)
                side_bar_position.X -= panel_delta.X / 2.0f;
            else
                side_bar_position.X += panel_delta.X / 2.0f;
            FVector const side_bar_scale(
                hex_map_config.offmap_border_thickness,
                panel_scale.Y - hex_map_config.offmap_border_thickness,
                hex_map_config.offmap_border_thickness
            );
            offmap_borders_.add(
                nation.nation_id,
                FTransform(FRotator(0, 0, 0), side_bar_position, side_bar_scale)
            );

            FVector const top_bottom_bar_scale(
                panel_scale.X + hex_map_config.offmap_border_thickness,
                hex_map_config.offmap_border_thickness,
                hex_map_config.offmap_border_thickness
            );
            auto top_bottom_bar_position = panel_position;
            top_bottom_bar_position.Y += panel_delta.Y / 2.0f;
            offmap_borders_.add(
                nation.nation_id,
                FTransform(FRotator(0, 0, 0), top_bottom_bar_position, top_bottom_bar_scale)
            );
            top_bottom_bar_position.Y -= panel_delta.Y;
            offmap_borders_.add(
                nation.nation_id,
                FTransform(FRotator(0, 0, 0), top_bottom_bar_position, top_bottom_bar_scale)
            );
        } else {
            auto top_bottom_bar_position = panel_position;
            if (top)
                top_bottom_bar_position.Y += panel_delta.Y / 2.0f;
            else
                top_bottom_bar_position.Y -= panel_delta.Y / 2.0f;
            FVector const top_bottom_bar_scale(
                panel_scale.X - hex_map_config.offmap_border_thickness,
                hex_map_config.offmap_border_thickness,
                hex_map_config.offmap_border_thickness
            );
            offmap_borders_.add(
                nation.nation_id,
                FTransform(FRotator(0, 0, 0), top_bottom_bar_position, top_bottom_bar_scale)
            );

            FVector const side_bar_scale(
                hex_map_config.offmap_border_thickness,
                panel_scale.Y + hex_map_config.offmap_border_thickness,
                hex_map_config.offmap_border_thickness
            );
            auto side_bar_position = panel_position;
            side_bar_position.X += panel_delta.X / 2.0f;
            offmap_borders_.add(
                nation.nation_id,
                FTransform(FRotator(0, 0, 0), side_bar_position, side_bar_scale)
            );
            side_bar_position.X -= panel_delta.X;
            offmap_borders_.add(
                nation.nation_id,
                FTransform(FRotator(0, 0, 0), side_bar_position, side_bar_scale)
            );
        }

        if (auto & text_render_component = offmap_labels_[nation.nation_id]) {
            text_render_component->SetVisibility(true);
            text_render_component->SetText(offmap_area.name.c_str());
            text_render_component->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
            text_render_component->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
            text_render_component->SetWorldSize(hex_map_config.offmap_label_size);
            text_render_component->SetTextMaterial(text_mat_);

            auto text_position = panel_position;
            text_position.Z = hex_map_config.offmap_z * meters;
            if (top)
                text_position.Y = upper_right.Y - offmap_top_bottom_gap / 2.0f;
            else if (bottom)
                text_position.Y = lower_left.Y + offmap_top_bottom_gap / 2.0f;
            else if (left)
                text_position.X = lower_left.X + offmap_left_right_gap / 2.0f;
            else if (right)
                text_position.X = upper_right.X - offmap_left_right_gap / 2.0f;

            float z_rotation = 0.0f;
            if (left)
                z_rotation = 180.0f;
            else if (top || bottom)
                z_rotation = 90.0f;

            text_render_component->SetWorldLocationAndRotation(text_position, FRotator(-90, 0, z_rotation));

            FColor const color = to_fcolor(hex_map_config.secondary_colors.find(pair.first)->second);
            text_render_component->SetTextRenderColor(color);
        }
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
        auto const id = hex_id_t(map_hex.coord);
        capital_hex = std::count_if(
            nation.capital.hexes.begin(), nation.capital.hexes.end(),
            [=](start_data::capital_hex_t const & capital_hex) { return capital_hex.coord == id.to_int(); }
        );
        star_points = nation.capital_star_points;
    }

    auto const & hex_map_config = visual_config_.hex_map();

    if (capital_hex) {
        FTransform const transform(
            rotation,
            location,
            FVector(
                hex_map_config.star_scale,
                hex_map_config.star_scale,
                hex_map_config.planet_star_thickness
            )
        );
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
        if (planet_type == planet_t::type_t::minor) {
            transform = FTransform(
                rotation,
                location,
                FVector(
                    hex_map_config.minor_planet_scale,
                    hex_map_config.minor_planet_scale,
                    hex_map_config.planet_star_thickness
                )
            );
        } else {
            transform = FTransform(
                rotation,
                location,
                FVector(
                    hex_map_config.major_planet_scale,
                    hex_map_config.major_planet_scale,
                    hex_map_config.planet_star_thickness
                )
            );
        }
        // TODO: Adjust for the presence of sb, bats, or mb?
        planets_.add(owner_id, transform);
    } else if (0 < sbs) {
        FTransform const transform(
            rotation,
            location,
            FVector(
                hex_map_config.sb_scale,
                hex_map_config.sb_scale,
                hex_map_config.planet_star_thickness
            )
        );
        // TODO: Adjust for the presence of other sb, bats, or mb?
        starbases_.add(owner_id, transform);
#if 1 // For testing only!
        FVector fleet_location = location;
        fleet_location.Z = -0.25f * meters;

        FRotator const rotation = {0.0f, 0.0f, 0.0f};

        FActorSpawnParameters spawn_params;
        spawn_params.Owner = this;
        spawn_params.Instigator = Instigator;

        UWorld * const world = GetWorld();
        world->SpawnActor<AActor>(large_fleet_actor_, fleet_location, rotation, spawn_params);
#endif
    } else if (0 < batss) {
        FTransform const transform(
            rotation,
            location,
            FVector(
                hex_map_config.bats_scale,
                hex_map_config.bats_scale,
                hex_map_config.planet_star_thickness
            )
        );
        // TODO: Adjust for the presence of other sb, bats, or mb?
        battlestations_.add(owner_id, transform);
#if 1 // For testing only!
        FVector fleet_location = location;
        fleet_location.Z = -0.25f * meters;

        FRotator const rotation = {0.0f, 0.0f, 0.0f};

        FActorSpawnParameters spawn_params;
        spawn_params.Owner = this;
        spawn_params.Instigator = Instigator;

        UWorld * const world = GetWorld();
        world->SpawnActor<AActor>(large_fleet_actor_, fleet_location, rotation, spawn_params);
#endif
    } else if (0 < mbs) {
        FTransform const transform(
            rotation,
            location,
            FVector(
                hex_map_config.mb_scale,
                hex_map_config.mb_scale,
                hex_map_config.planet_star_thickness
            )
        );
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
