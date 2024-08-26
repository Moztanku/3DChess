#pragma once

#include <cstdint>

namespace Controller
{

enum class GameState : uint8_t
{
    Playing = 0,
    WhiteWin,
    BlackWin,
    Draw
};

} // namespace Controller
