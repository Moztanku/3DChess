#pragma once

#include <string_view>
#include <unordered_map>

#include <glm/vec2.hpp>

#include "Renderer/Chessboard.hpp"

#include "Chess/Common.hpp"
#include "Chess/Piece.hpp"

namespace Chess
{

class Board
{
    public:
        Board(const std::string_view layout_file);

        auto get_size() const -> Pos { return Pos{m_width, m_height}; }
        auto get_piece(Pos where) const -> const Piece*;
        auto get_king(Player color) const -> const King* { return color == Player::White ? m_KingWhite : m_KingBlack; }

        auto move_piece(Pos from, Pos to) -> void;

        auto draw(const glm::mat4& projView) const -> void;

        static auto get_board() -> const Board&;
    private:
        std::unordered_map<
            Pos,
            Piece*,
            PosKeyFuncs> m_pieces;
        King* m_KingWhite{};
        King* m_KingBlack{};

        // TODO: Change chessboard class into a Mesh class
        Renderer::Chessboard* m_chessboard;
        
        // Board dimensions, pseudo-constants as they are set in the constructor
        uint m_width;
        uint m_height;

        // Static pointer to the board, needed so that the pieces can access the board
        static Board* s_board;
}; // class Board

} // namespace Chess
