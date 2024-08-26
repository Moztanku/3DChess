#pragma once

#include <glm/vec2.hpp>

#include "jac/type_defs.hpp"

namespace Chess
{

using Pos = glm::vec<2, int>;

// Check if a position is within given bounds
constexpr auto in_bounds(const Chess::Pos pos, const Chess::Pos bounds) -> bool
{
    return pos.x >= 0 && pos.x < bounds.x
        && pos.y >= 0 && pos.y < bounds.y;
}

// Custom hash function for Pos to be used in std::unordered_map
struct PosKeyFuncs
{
    auto operator()(const Pos& pos) const -> size_t
    {
        return std::hash<int>()(pos.x) ^ std::hash<int>()(pos.y);
    };
};

enum class Player : uint8_t
{
    White = 0,
    Black = 1
};

constexpr auto operator!(const Player player) -> Player
{
    return player == Player::White ? Player::Black : Player::White;
}

}; // namespace Chess
