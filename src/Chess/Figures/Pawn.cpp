#include "Chess/Piece.hpp"

#include "Chess/Board.hpp"

namespace Chess
{

Pawn::Pawn(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Pawn::get_moves() const -> std::vector<Pos>
{
    const Board& board = Board::get_board();
    std::vector<Pos> moves;

    const int forward = 
        m_color == Player::White ? 1 : -1;

    // Move forward
    Pos next_pos = m_pos + Pos{0, forward};

    if (board.get_piece(next_pos) == nullptr)
    {
        moves.push_back(next_pos);

        // Move two squares forward
        if (!m_moved)
        {
            next_pos = m_pos + Pos{0, forward * 2};

            if (board.get_piece(next_pos) == nullptr)
            {
                moves.push_back(next_pos);
            }
        }
    }

    // Capture
    for (int x = -1; x <= 1; x += 2)
    {
        next_pos = m_pos + Pos{x, forward};

        if (board.get_piece(next_pos) != nullptr)
        {
            moves.push_back(next_pos);
        }
    }

    // En passant
    // TODO

    return moves;
}

auto Pawn::get_type() const -> std::string
{
    return "Pawn";
}

auto Pawn::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/pawn/model.obj"
    };

    return mesh;
}

} // namespace Chess
