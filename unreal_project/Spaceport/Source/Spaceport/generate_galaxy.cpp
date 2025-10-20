#include "constants.hpp"
#include "generate_galaxy.hpp"
#include "game_data_t.hpp"
#include "rng.hpp"

#include <numbers>


namespace {
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

    // See: https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_law
    // Luminance formula: L = 4piR^2σT^4
    // Simplified in terms of solar factors: R = T^-2L^0.5 = L^0.5 / T^2
    double solar_radius(double L, double T)
    {
        return std::sqrt(L) / (T * T);
    }

    constexpr star_property_ranges_t star_properties[] = {
        {star_class_t::invalid_star_class, 0.0, {}, {}, {}},
        {star_class_t::o, 0.0000003, {33000, 1000000}, {16, 1000000}, {30000, 1000000}},
        {star_class_t::b, 0.0012, {10000, 33000}, {2.1, 16}, {25, 30000}},
        {star_class_t::a, 0.0061, {7300, 10000}, {1.4, 2.1}, {5, 25}},
        {star_class_t::f, 0.03, {6000, 7300}, {1.04, 1.4}, {1.5, 5}},
        {star_class_t::g, 0.076, {5300, 6000}, {0.8, 1.04}, {0.6, 1.5}},
        {star_class_t::k, 0.12, {3900, 5300}, {0.45, 0.8}, {0.08, 0.6}},
        {star_class_t::m, 0.76, {2300, 3900}, {0.08, 0.45}, {0.01, 0.08}}
    };

    // See: https://en.wikipedia.org/wiki/Sun
    constexpr double earth_to_sun_km = 150000000;
    constexpr double sun_temperature_k = 5772.0;

    double degrees_to_radians(double d)
    {
        return d / 180.0 * std::numbers::pi;
    }

    struct point_2d
    {
        double x;
        double y;
    };

    point_2d operator-(point_2d pt1, point_2d pt2)
    {
        return {pt1.x - pt2.x, pt1.y - pt2.y};
    }

    bool within(point_2d pt1, point_2d pt2, double dist)
    {
        auto const delta = pt1 - pt2;
        return (delta.x * delta.x + delta.y * delta.y) < (dist * dist + 0.1);
    }

    point_2d hex_position(hex_coord_t hc, int map_height)
    {
        point_2d retval;
        retval.x = hc.x * 1.5;
        retval.y = (map_height - 1 - hc.y) * 2 * sin_60;
        if ((hc.x + 1000) % 2 == 1)
            retval.y -= sin_60;
        return retval;
    }

    star_t generate_star()
    {
        star_t retval;

        // TODO: See if generating 100s or 1000s of rolls at once is faster.
        double roll = random_unit_double();
        auto it = std::ranges::find_if(
            star_properties,
            [&](auto const & elem) {
                bool const retval = roll < elem.frequency_;
                roll -= elem.frequency_;
                return retval;
            });
        if (it == std::ranges::begin(star_properties) ||
            it == std::ranges::end(star_properties)) {
            it = std::ranges::begin(star_properties) + 5;
        }
        retval.star_class = it->class_;

        auto const & ranges = *it;
        retval.temperature_k = std::lerp(ranges.temperature_.first,
                                         ranges.temperature_.second,
                                         random_unit_double());
        retval.solar_masses = std::lerp(ranges.mass_.first,
                                        ranges.mass_.second,
                                        random_unit_double());
        retval.solar_luminosities = std::lerp(ranges.luminosity_.first,
                                              ranges.luminosity_.second,
                                              random_unit_double());

        retval.solar_radii = solar_radius(
            retval.solar_luminosities,
            retval.temperature_k / sun_temperature_k);

        return retval;
    }

    // Mercury 5427 kg/km^3 3.3e23 kg
    // Venus 5243 kg/km^3 4.87e24 kg
    // Earth 5514 kg/km^3 5.97e24 kg
    // Mars 3934 kg/km^3 6.42e23 kg
    // Jupiter 1326 kg/km^3 1.90e27 kg
    // Saturn 687 kg/km^3 5.68e26 kg
    // Uranus 1270 kg/km^3 1.02e26 kg
    // Neptune 1638 kg/km^3 8.68e25 kg

    bool generate_planet(planet_t & planet, system_t const & system)
    {
        // According to Google, planets < 0.3 Jupiter masses are rocky.
        constexpr double rockiness_mass_threshold = 5.7e26;

        std::gamma_distribution<double> day_dist(2, 1.8);
        std::normal_distribution<double> tilt_dist(0.0, 30.0);
        std::normal_distribution<double> rocky_density_dist(5250.0, 400.0);
        // also used for ice giants.
        std::normal_distribution<double> gas_giant_density_dist(1000.0, 150.0);
        std::normal_distribution<double> ice_giant_density_dist(1400.0, 150.0);

        // Arbitrary line between the gas giants and the ice giants, taken
        // from the boundary between the Saturn and Uranus orbits, and scaled
        // by solar masses of this system's star..
        double const ice_distance_au = 15.0 * system.star.solar_masses;

        double density = 0.0; // in kg/km^3
        if (planet.mass_kg < rockiness_mass_threshold) {
            density = random_number(rocky_density_dist);
            planet.planet_type = planet_type_t::rocky;
        } else if (planet.orbit_au < ice_distance_au) {
            density = random_number(gas_giant_density_dist);
            planet.planet_type = planet_type_t::gas_giant;
        } else {
            density = random_number(ice_giant_density_dist);
            planet.planet_type = planet_type_t::ice_giant;
        }
        double const volume = planet.mass_kg / density;

        planet.radius_km = std::cbrt(0.75 * volume / std::numbers::pi);

        // 1000 to convert to meters
        double const surface_gravity =
            G * planet.mass_kg / (planet.radius_km * 1000.0);
        planet.gravity_g = surface_gravity / earth_gravity;

        planet.axial_tilt_d =
            std::min(std::abs(random_number(tilt_dist)), 90.0);

        // 15 is just a made up number.
        planet.day_h = 15.0 * random_number(day_dist);

        // TODO: resources

        return false; // TODO: return habitability
    }

    bool generage_system(
        system_t & system, std::vector<planet_t> & planets,
        hex_coord_t hc, point_2d hex_world_pos, int system_id)
    {
        // system.name = TODO;

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

        // Now the corners on the left are at x=-0.5 to x=0.0; correct.
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
                radii.back() * (1.0 + random_number(gamma_dist));
            radii.push_back(next_pos_au);
        }
        radii.back() = std::min(radii.back(), protostar_disk_radius_au);

        // Create area/masses for each planet.
        // Also, replace radii with midpoint between near/far.
        std::vector<double> masses(radii.size());
        double sum_of_masses = 0.0;
        std::transform(radii.begin(), radii.end() - 1, radii.begin() + 1,
                       masses.begin(), [&](double & near, double far) {
                           // Area of the accretion disk that was involved in
                           // forming this planet.
                           double const retval =
                               std::numbers::pi * (far * far - near * near);
                           sum_of_masses += retval;
                           near = (near + far) / 2.0;
                           return retval;
                       });
        radii.pop_back();

        // Replace masses with fractions of sum of mass of all planets, then
        // scale that to kg based on the Solar sytem.
        std::transform(
            masses.begin(), masses.end(), masses.begin(), [&](double mass) {
                return mass / sum_of_masses *
                    mass_of_solar_system_planets_kg * system.star.solar_masses;
            });

        planets.resize(planets.size() + masses.size());
        system.last_planet = planets.size();

        std::ranges::subrange const system_planets(
            planets.end() -  masses.size(), planets.end());
        bool has_habitable_planet = false;
        auto radii_it = radii.begin();
        auto masses_it = masses.begin();
        for (auto & planet : system_planets) {
            planet.system_id = system_id;
            planet.orbit_au = *radii_it++;
            planet.mass_kg = *masses_it++;
            if (generate_planet(planet, system))
                has_habitable_planet = true;
        }

        return has_habitable_planet;
    }
}

void generate_galaxy(game_start_params const & params,
                     game_state_t & game_state)
{
    double const map_radius = params.map_height / 2.0;
    double const bulge_radius = map_radius / 10.0;

    game_state.map_height = params.map_height;
    game_state.map_width =
        2 * int(std::ceil(map_radius * (hex_width / hex_height)));
    if (game_state.map_width % 2 == 0)
        ++game_state.map_width;
    game_state.hexes.resize(game_state.map_width * game_state.map_height);

    auto const hex_position_ =
        [map_height = game_state.map_height](hex_coord_t hc) {
            return ::hex_position(hc, map_height);
        };

   hex_coord_t const center_hex{
       game_state.map_width / 2, game_state.map_height / 2};
   point_2d const center_hex_pos = hex_position_(center_hex);

   std::normal_distribution<double> habitable_systems_dist(
       params.habtitable_systems_per_hex_mean,
       params.habtitable_systems_per_hex_std_dev);

   int hex_index = 0;
   for (auto & hex : game_state.hexes) {
       hex_coord_t const hc{
           hex_index % game_state.map_width, hex_index / game_state.map_width};
       hex.coord = hc;

       auto const pos = hex_position_(hc);
       if (within(center_hex_pos, pos, bulge_radius)) {
           if (hc == center_hex)
               hex.province_id = galactic_center;
           else
               hex.province_id = galactic_bulge;
           continue;
       } else if (!within(center_hex_pos, pos, map_radius)) {
           hex.province_id = off_map;
           continue;
       }

       hex.province_id = no_province;
       hex.first_system = game_state.systems.size();

       auto const habitable_systems =
           int(std::round(random_number(habitable_systems_dist)));
       int habitable_systems_so_far = 0;
       int system_id = hex.first_system;
       while ((habitable_systems &&
               habitable_systems_so_far < habitable_systems) ||
              (!habitable_systems &&
               game_state.systems.size() - hex.first_system < 20u)) {
           ++system_id;
           game_state.systems.push_back(system_t());
           if (generage_system(
                   game_state.systems.back(), game_state.planets,
                   hc, pos, system_id)) {
               ++habitable_systems_so_far;
           }
       }

       hex.last_system = game_state.systems.size();
   }
}
