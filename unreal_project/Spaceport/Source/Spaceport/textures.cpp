#include "textures.h"

#include "Aplayer_controller.h"
#include "rng.hpp"
#include "utility.hpp"


int Utextures_t::random_wide_lens_flare(detail::rng_state & state) const
{
    return random_int(0, 4, state);
}

int Utextures_t::random_small_lens_flare(detail::rng_state & state) const
{
    return random_int(0, 1, state);
}

UTexture * Utextures_t::wide_lens_flare(int i) const
{
    std::array<UTexture *, 5> textures = {
        {wide_lens_flare_0_,
         wide_lens_flare_1_,
         wide_lens_flare_2_,
         wide_lens_flare_3_,
         wide_lens_flare_4_}};
    return textures[std::clamp(i, 0, 4)];
}

UTexture * Utextures_t::small_lens_flare(int i) const
{
    std::array<UTexture *, 2> textures = {
        {small_lens_flare_0_, small_lens_flare_1_}};
    return textures[std::clamp(i, 0, 1)];
}

UTexture * Utextures_t::random_planet_texture(detail::rng_state & state) const
{
    std::array<UTexture *, 9> textures = {
        {planet_texture_0_,
         planet_texture_1_,
         planet_texture_2_,
         planet_texture_3_,
         planet_texture_4_,
         planet_texture_5_,
         planet_texture_6_,
         planet_texture_7_,
         planet_texture_8_}};
    return textures[random_int(0, 8, state)];
}

Utextures_t const & textures()
{
    auto * pc = player_controller();
    check(pc);
    return pc->textures();
}
