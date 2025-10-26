#pragma once

#include "constants.hpp"
#include "game_data.hpp"
#include "rng.hpp"

#include <numbers>


struct game_start_params;
struct game_state_t;

// TODO: Move these.
struct point_2d
{
    double x;
    double y;
};

inline point_2d operator-(point_2d pt1, point_2d pt2)
{
    return {pt1.x - pt2.x, pt1.y - pt2.y};
}

inline bool within(point_2d pt1, point_2d pt2, double dist)
{
    auto const delta = pt1 - pt2;
    return (delta.x * delta.x + delta.y * delta.y + 0.001) < (dist * dist);
}

inline point_2d hex_position(hex_coord_t hc, int map_height)
{
    point_2d retval;
    retval.x = hc.x * 1.5;
    retval.y = (map_height - 1 - hc.y) * 2 * sin_60;
    if ((hc.x + 1000) % 2 == 1)
        retval.y -= sin_60;
    return retval;
}

inline double degrees_to_radians(double d)
{
    return d / 180.0 * std::numbers::pi;
}

namespace generation {
    namespace detail {
        // https://en.wikipedia.org/wiki/Stellar_classification

        // All units are solar (solar masses, solar luminosities, etc.).
        struct star_property_ranges_t
        {
            star_class_t class_;
            double frequency_;
            std::pair<double, double> temperature_;
            std::pair<double, double> mass_;
            std::pair<double, double> luminosity_;
        };

        constexpr star_property_ranges_t star_properties[] = {
            {star_class_t::invalid_star_class, 0.0, {}, {}, {}},
            {star_class_t::o, 0.0000003, {33000, 1000000},
             {16, 1000000}, {30000, 1000000}},
            {star_class_t::b, 0.0012, {10000, 33000}, {2.1, 16}, {25, 30000}},
            {star_class_t::a, 0.0061, {7300, 10000}, {1.4, 2.1}, {5, 25}},
            {star_class_t::f, 0.03, {6000, 7300}, {1.04, 1.4}, {1.5, 5}},
            {star_class_t::g, 0.076, {5300, 6000}, {0.8, 1.04}, {0.6, 1.5}},
            {star_class_t::k, 0.12, {3900, 5300}, {0.45, 0.8}, {0.08, 0.6}},
            {star_class_t::m, 0.76, {2300, 3900}, {0.08, 0.45}, {0.01, 0.08}}
        };

        // See: https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_law
        // Luminance formula: L = 4piR^2σT^4
        // Simplified in terms of solar factors: R = T^-2L^0.5 = L^0.5 / T^2
        inline double solar_radius(double L, double T)
        {
            return std::sqrt(L) / (T * T);
        }

        // TODO: Move this.
        inline std::string roman_number(int num)
        {
            if (num < 1 || 3999 < num)
                return "SOME_ROMAN_NUMBER";
            using namespace std::literals;
            std::pair<int, std::string_view> const all_numerals[] = {
                {1000, "M"sv}, {900, "CM"sv}, {500, "D"sv}, {400, "CD"sv},
                {100, "C"sv}, {90, "XC"sv}, {50, "L"sv}, {40, "XL"sv},
                {10, "X"sv}, {9, "IX"sv}, {5, "V"sv}, {4, "IV"sv}, {1, "I"sv}
            };
            std::string retval;
            for (auto [value, numerals] : all_numerals) {
                while (value <= num) {
                    retval += numerals;
                    num -= value;
                }
            }
            return retval;
        }

        float determine_growth_factor_and_effects(planet_t & planet);

        bool generate_planet(planet_t & planet, system_t const & system);

        inline bool generate_system_planets(
            system_t & system, int system_id, std::vector<double> const & radii,
            std::vector<double> const & masses,
            std::ranges::subrange<std::vector<planet_t>::iterator> system_planets)
        {
            bool has_habitable_planet = false;
            auto radii_it = radii.begin();
            auto masses_it = masses.begin();
            for (auto & planet : system_planets) {
                planet.system_id = system_id;
                planet.orbit_au = *radii_it++;
                planet.mass_kg = *masses_it++;

                double const orbital_radius_m =
                    1000.0 * planet.orbit_au * au_to_km;
                double const orbital_period_s = std::sqrt(
                    4 * std::numbers::pi * std::numbers::pi *
                    orbital_radius_m * orbital_radius_m * orbital_radius_m /
                    G * (system.star.solar_masses * sun_mass_kg +
                         planet.mass_kg));
                planet.orbital_period_y = orbital_period_s / years_to_seconds;

                if (generate_planet(planet, system))
                    has_habitable_planet = true;
            }

            return has_habitable_planet;
        }

        star_t generate_star(double roll = random_unit_double());

#if !defined(BUILD_FOR_TEST)
        // TODO: Call this with TEXT("star_names") and put the result
        // somewhere central.
        inline TArray<FName> get_string_table_keys(TCHAR table)
        {
            FStringTablePtr const string_table =
                FStringTableRegistry::Get().FindStringTable(table);
            TArray<FName> string_table_keys;
            if (!string_table.IsValid())
                return {};
            string_table->GetKeys(string_table_keys);
            return std::move(string_table_keys);
        }

        inline adobe::name_t new_star_name(
            int index, TArray<FName> const & string_table_keys)
        {
            int const size = string_table_keys.Num();
            if (!size)
                return adobe::name_t("Invalid 'star_names' string table");

            int const cycles = index / size;
            bool const needs_new_prefix = cycles == 1;
            if (1 < cycles)
                return adobe::name_t("Ran out of star names");
            index %= size;

            FName const key = string_table_keys[random_int(0, size - 1)];
            if (key.IsNone())
                return adobe::name_t("Null key in 'star_names' string table");

            FString const value = string_table->GetTableEntry(key).ToString();
            std::string string_copy = TCHAR_TO_UTF8(*value);

            if (needs_new_prefix) {
                // TODO: Make a new system name by jamming the localized "New
                // " on the front of this index's name.
            }

            return adobe::name_t(string_copy.c_str());
        }
#endif

        template<typename GenPlanetsFn>
        bool generate_system_impl(
            system_t & system, std::vector<planet_t> & planets,
            hex_coord_t hc, point_2d hex_world_pos, int system_id,
            GenPlanetsFn && gen_planets)
        {
#if !defined(BUILD_FOR_TEST)
            // TODO: Consider only giving a star a name once it has a colony
            // or other permanent assets in it.  Another possibility would be
            // to only give habitable systems names.

            // TODO: Need a stored star_name_string_table_keys somewhere.
            // Create this with get_string_table_keys above.
            system.name = new_star_name(system_id, star_name_string_table_keys);
#endif

            system.coord = hc;
            system.star = generate_star();

            system.first_planet = planets.size();

            // TODO: See if generating 100s or 1000s of rolls at once is faster.
            double x_roll = random_unit_double();
            double y_roll = hex_height * random_unit_double();

            /* The (x,y) rolled above is mapped onto a rectangle that represents
               all the locations within the hex.  Since a hex does not exactly
               cover a rectangle, the left-side corners are snipped off and put on
               the right to form the rectangle, as shown below.

               |-----|
               |   \ | <- missing lower left segment of hex
               |    \|
               |    /|
               |   / | <- missing upper left segment of hex
               |-----|
            */

            if (0.5 < x_roll) {
                if (hex_height / 2.0 < y_roll)
                    y_roll -= hex_height / 2.0;
                else
                    y_roll += hex_height / 2.0;
                x_roll -= 1.0;
            }

            // Now the corners on the left are at x=-0.5 to x=0.0; correct
            // this.
            x_roll += 0.5;

            // Change coord to be relative to hex center. TODO?
            //x_roll -= hex_width / 2.0;
            //y_roll -= hex_height / 2.0;

            system.world_pos_x = hex_world_pos.x + x_roll;
            system.world_pos_y = hex_world_pos.y + y_roll;

            // 35 comes from Neptune's orbit at about 30AU, and adding a little.
            // The * solar_masses is there because the size of the accretion disk
            // that formed this system is vaguely proportional to the mass of the
            // star.
            double const protostar_disk_radius_au =
                35.0 * system.star.solar_masses;
            std::gamma_distribution<double> gamma_dist(1.0, 1.5);

            std::vector<double> radii; // in AU
            radii.push_back(0.25 * system.star.solar_masses);
            while (radii.back() < protostar_disk_radius_au) {
                double next_pos_au =
                    radii.back() * (1 + random_number(gamma_dist));
                radii.push_back(next_pos_au);
            }
            radii.back() = std::min(radii.back(), protostar_disk_radius_au);

            // Treat the accretion disk as uniform density, but having
            // thickness that follows 1/R, where R is radius from the star.
            // This means the area under 1/R is the relative amount of mass
            // used in a Rnear to Rfar slice of the disk.  The multiplication
            // at the end is simly there so that the effect is not so
            // dramatic.
            auto const disk_curve_area_under = [](double near, double far) {
                return (std::log(far) - std::log(near)) * 0.5;
            };

            // Create area/masses for each planet.
            // Also, replace radii with midpoint between near/far.
            std::vector<double> masses(radii.size() - 1);
            double sum_of_masses = 0.0;
            std::transform(radii.begin(), radii.end() - 1, radii.begin() + 1,
                           masses.begin(), [&](double & near, double far) {
                               // Volume of the accretion disk that was involved in
                               // forming this planet.
                               double const vertical_area =
                                   disk_curve_area_under(near, far);
                               double const retval = vertical_area *
                                   std::numbers::pi * (far * far - near * near);
                               sum_of_masses += retval;
                               near = (near + far) / 2.0;
                               return retval;
                           });
            radii.pop_back();

            // Replace masses with fractions of the sum of the masses of all
            // the planets, then scale that to kg based on the Solar sytem.
            std::transform(
                masses.begin(), masses.end(), masses.begin(), [&](double mass) {
                    return mass / sum_of_masses *
                        mass_of_solar_system_planets_kg * system.star.solar_masses;
                });

            planets.resize(planets.size() + masses.size());
            system.last_planet = planets.size();

            return gen_planets(
                system, system_id, radii, masses,
                std::ranges::subrange(planets.end() -  masses.size(), planets.end()));
        }

        inline bool generate_system(
            system_t & system, std::vector<planet_t> & planets,
            hex_coord_t hc, point_2d hex_world_pos, int system_id)
        {
            return generate_system_impl(
                system, planets, hc, hex_world_pos, system_id,
                &generate_system_planets);
        }

#if defined(BUILD_FOR_TEST)
        inline thread_local bool g_skip_system_generation_for_testing = false;
#endif
    }

    void generate_galaxy(game_start_params const & params,
                         game_state_t & game_state);
}

// TODO: Move this.
inline void resort_effects(planet_t & planet)
{
    std::stable_sort(
        planet.effects.begin(), planet.effects.end(),
        [](planet_effect_t const & a, planet_effect_t const & b) {
            return std::pair((int)a.target, a.target_modifiers) <
                std::pair((int)b.target, b.target_modifiers);
        });
}
