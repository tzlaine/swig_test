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

        star_t generate_star(double roll = random_unit_double());

        bool generate_system(
            system_t & system, std::vector<planet_t> & planets,
            hex_coord_t hc, point_2d hex_world_pos, int system_id);

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
