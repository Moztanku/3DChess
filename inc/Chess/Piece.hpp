#pragma once

#include <vector>

#include "Chess/Common.hpp"

#include "Renderer/Mesh.hpp"

namespace Chess
{

struct Piece
{
    enum class Type
    {
        Pawn = 0,
        Bishop,
        Knight,
        Rook,
        Queen,
        King
    };

    Player color;
    Type type;
    bool moved{false};
};

} // namespace Chess
