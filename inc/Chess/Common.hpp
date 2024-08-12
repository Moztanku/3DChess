#pragma once

#include <glm/vec2.hpp>

#include "jac/type_defs.hpp"

namespace Chess
{

using Pos = glm::vec<2, int>;

// Custom hash function for Pos to be used in std::unordered_map
struct PosKeyFuncs
{
    auto operator()(const Pos& pos) const -> size_t
    {
        return std::hash<int>()(pos.x) ^ std::hash<int>()(pos.y);
    };
};

enum class Player
{
    White,
    Black
};

}; // namespace Chess
