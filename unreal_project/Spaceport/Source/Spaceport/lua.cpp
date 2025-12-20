#include "lua.hpp"

#include "constants.hpp"
#include "game_data_metadata.hpp"

#if defined(BUILD_FOR_TEST)
#include <iostream>
#else
#include <CoreMinimal.h>
#include <Misc/Paths.h>
#endif


namespace {
    void ue_log_impl(std::string const & msg)
    {
#if defined(BUILD_FOR_TEST)
        std::cout << "LOG: " << msg << "\n";
#else
        UE_LOG(LogTemp, Log, TEXT("%s"), *FString(UTF8_TO_TCHAR(msg.c_str())));
#endif
    }
    void ue_warn_impl(std::string const & msg)

    {
#if defined(BUILD_FOR_TEST)
        std::cout << "WARNING: " << msg << "\n";
#else
        UE_LOG(
            LogTemp, Warning, TEXT("%s"), *FString(UTF8_TO_TCHAR(msg.c_str())));
#endif
    }
    void ue_err_impl(std::string const & msg)
    {
#if defined(BUILD_FOR_TEST)
        std::cout << "ERROR: " << msg << "\n";
#else
        UE_LOG(
            LogTemp, Error, TEXT("%s"), *FString(UTF8_TO_TCHAR(msg.c_str())));
#endif
    }

    void lua_register_physical_constants(sol::state & l)
    {
#define REGISTER_PHYSICAL_CONSTANT(x) l[#x] = x

        REGISTER_PHYSICAL_CONSTANT(G_);
        REGISTER_PHYSICAL_CONSTANT(sun_temperature_k);
        REGISTER_PHYSICAL_CONSTANT(sun_radius_km);
        REGISTER_PHYSICAL_CONSTANT(sun_mass_kg);
        REGISTER_PHYSICAL_CONSTANT(m_per_km);
        REGISTER_PHYSICAL_CONSTANT(km_per_au);
        REGISTER_PHYSICAL_CONSTANT(secs_per_year);
        REGISTER_PHYSICAL_CONSTANT(earth_temperature_k);
        REGISTER_PHYSICAL_CONSTANT(earth_mass_kg);
        REGISTER_PHYSICAL_CONSTANT(earth_gravity);
        REGISTER_PHYSICAL_CONSTANT(earth_radius_km);
        REGISTER_PHYSICAL_CONSTANT(earth_ocean_coverage);
        REGISTER_PHYSICAL_CONSTANT(earth_o2_percentage);
        REGISTER_PHYSICAL_CONSTANT(harmless_low_o2_percentage);
        REGISTER_PHYSICAL_CONSTANT(effective_o2_percentage_la_paz_bolivia);
        REGISTER_PHYSICAL_CONSTANT(effective_o2_percentage_aconcagua);
        REGISTER_PHYSICAL_CONSTANT(effective_o2_percentage_mt_everest_peak);
        REGISTER_PHYSICAL_CONSTANT(mass_of_solar_system_planets_kg);
        REGISTER_PHYSICAL_CONSTANT(sin_60);
        REGISTER_PHYSICAL_CONSTANT(epsilon);
        REGISTER_PHYSICAL_CONSTANT(hex_width);
        REGISTER_PHYSICAL_CONSTANT(hex_height);

#undef REGISTER_PHYSICAL_CONSTANT
    }

    void lua_register_sentinels(sol::state & l)
    {
#define REGISTER_SENTINEL(x) l[#x] = make_##x()

        REGISTER_SENTINEL(prov_none);
        REGISTER_SENTINEL(prov_off_map);
        REGISTER_SENTINEL(prov_galactic_bulge);
        REGISTER_SENTINEL(prov_galactic_center);
        REGISTER_SENTINEL(atmos_thousands);
        REGISTER_SENTINEL(atmos_millions);
        REGISTER_SENTINEL(growth_uninhabitable);
        REGISTER_SENTINEL(nation_none);
        REGISTER_SENTINEL(hex_none);
        REGISTER_SENTINEL(system_none);
        REGISTER_SENTINEL(object_none);
        REGISTER_SENTINEL(n_a);

#undef REGISTER_SENTINEL
    }
}


sol::state make_lua_state()
{
    sol::state retval;

    retval.open_libraries(
        sol::lib::base,
        sol::lib::package,
        sol::lib::string,
        sol::lib::math,
        sol::lib::table);

    retval.set_function("UE_LOG", &ue_log_impl);
    retval.set_function("UE_WARN", &ue_warn_impl);
    retval.set_function("UE_ERR", &ue_err_impl);

    lua_register_physical_constants(retval);
    lua_register_sentinels(retval);

    // Load this first, since it contains the lowest-level cosntant calues
    // that are used everywhere else.
    retval.script_file(script_path("constants.lua"));

    // TODO: constants that are not configurable, but are used in the Lua code

    REGISTER_GAME_DATA_TYPE(retval, hex_coord_t);
    retval.script("invalid_hex_coord = hex_coord_t.new(-1, -1)");

    REGISTER_GAME_DATA_TYPE(retval, nation_and_object_id_t);
    retval.script(
        "invalid_nation_and_object = nation_and_object_id_t.new(-1, -1)");

    REGISTER_GAME_DATA_TYPE(retval, game_start_params_t);
    // TODO: Put this in a file script.
    retval.script(
        R"(default_game_start_params = game_start_params_t.new()
default_game_start_params.habitable_systems_per_hex_mean = 5.0
default_game_start_params.habitable_systems_per_hex_plus_minus = 2.0
default_game_start_params.systems_per_hex = default_systems_per_hex
default_game_start_params.map_height = small_map_height
)");

    detail::lua_register_1(retval);
    detail::lua_register_2(retval);
    detail::lua_register_3(retval);
    detail::lua_register_4(retval);

    retval.script_file(script_path("effects.lua"));

    return retval;
}

sol::state & lua()
{
    static thread_local sol::state retval = make_lua_state();
    return retval;
}

std::string script_path(std::string const & script_)
{
#if defined(BUILD_FOR_TEST)
#if defined(_MSC_VER)
    return "../../unreal_project/Spaceport/Content/script/" + script_;
#else
#error "Need a CMake-build script_path() implementation for your paltform."
#endif
#else
    FString script(UTF8_TO_TCHAR(script_.c_str()));
    FString full_path =
        FPaths::Combine(*FPaths::ProjectContentDir(), TEXT("script"), *script);
    return std::string(TCHAR_TO_UTF8(*full_path));
#endif
}

void script_file(std::string const & script)
{
    lua().script_file(script_path(script));
}
