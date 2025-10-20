#pragma once

#include <cmath>
#include <numbers>


// non-province constants
inline constexpr int no_province = -1;
inline constexpr int off_map = -2;
inline constexpr int galactic_bulge = -3;
inline constexpr int galactic_center = -4;

inline const double sin_60 = std::sin(std::numbers::pi / 3.0);

inline constexpr double hex_width = 1.5;
inline const double hex_height = 2 * sin_60;

constexpr double G = 6.67e-11; // gravitaional constant, in N * m^2 / kg^2
constexpr double earth_gravity = 9.8; // in m / s^2
constexpr double mass_of_solar_system_planets_kg =
    3.3e23 + 4.87e24 + 5.97e24 + 6.42e23 +
    1.90e27 + 5.68e26 + 1.02e26 + 8.68e25;
