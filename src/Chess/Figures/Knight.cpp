#include "Chess/Piece.hpp"

#include "Chess/Board.hpp"

namespace Chess
{

Knight::Knight(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Knight::get_moves() const -> std::vector<Pos>
{
    const Board& board = Board::get_board();
    std::vector<Pos> moves;
    
    constexpr std::array<Pos, 8> directions = {
            Pos{-1, 2}, Pos{1, 2},
        Pos{-2, 1},             Pos{2, 1},
                // Horsey here
        Pos{-2, -1},            Pos{2, -1},
            Pos{-1, -2}, Pos{1, -2}
    };

    for (const Pos dir : directions)
    {
        Pos next = m_pos + dir;

        if ((next.x < 0 || next.x >= board.get_size().x)
        || (next.y < 0 || next.y >= board.get_size().y))
            continue;

        if (board.get_piece(next) == nullptr || board.get_piece(next)->get_color() != m_color)
            moves.push_back(next);
    }
    
    return moves;
}

auto Knight::get_type() const -> std::string
{
    return "Knight";
}

auto Knight::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/horse/model.obj"
    };

    return mesh;
}

} // namespace Chess
