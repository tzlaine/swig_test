#include "textures.h"

#include "Aplayer_controller_base.h"
#include "rng.hpp"
#include "utility.hpp"


UTexture * Utextures_t::random_wide_lens_flare() const
{
    std::array<UTexture *, 5> textures = {
        {wide_lens_flare_0_,
         wide_lens_flare_1_,
         wide_lens_flare_2_,
         wide_lens_flare_3_,
         wide_lens_flare_4_}};
    return textures[random_int(0, 4)];
}

UTexture * Utextures_t::random_small_lens_flare() const
{
    std::array<UTexture *, 2> textures = {
        {small_lens_flare_0_, small_lens_flare_1_}};
    return textures[random_int(0, 1)];
}

UTexture * Utextures_t::random_planet_texture() const
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
    return textures[random_int(0, 8)];
}

Utextures_t const & textures()
{
    auto * pc = player_controller_base();
    check(pc);
    return pc->textures();
}
