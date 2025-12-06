#pragma once

enum struct map_pawn_kind : unsigned int {
    unknown = 0,
    hex = 1 << 0,
    system = 1 << 1,
    fleet = 1 << 2,
    any = hex | system | fleet
};
