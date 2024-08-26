#pragma once

#include <optional>
#include <variant>

#include "Chess/Piece.hpp"
#include "Chess/Common.hpp"

namespace Chess
{

struct Move
{
    using SpecialMoveInfo = std::vector<std::byte>;

    enum class Type : uint8_t
    {
        Empty = 0,
        Capture = 1 << 0,
        Promotion = 1 << 1,
        Castling = 1 << 2,
        FirstMove = 1 << 3,
        EnPassant = 1 << 4,
        Check = 1 << 5,
        Checkmate = 1 << 6,
        Stalemate = 1 << 7
    };

    Player player;

    Pos from;
    Pos to;

    Piece::Type piece;

    // Multiple types possible for a move (e.g. a capture and a promotion)
    uint8_t type{static_cast<uint8_t>(Type::Empty)};

    // Additional information for special moves
    // - Capture: captured piece position and type (Pos, Piece)
    // - Promotion: promoted piece type (Piece::Type)
    // - Castling: rook from and to positions (Pos, Pos)
    // not needed for rest, but we keep type info about FirstMove, EnPassant, Check, Checkmate
    // for control purposes.
    SpecialMoveInfo special_move_info;

    auto is_type(Type type) const -> bool { return (this->type & static_cast<uint8_t>(type)) != 0; }
};

} // namespace Chess
