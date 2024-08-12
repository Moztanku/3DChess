#include "Chess/Piece.hpp"

#include "Chess/Board.hpp"

namespace Chess
{

King::King(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto King::get_moves() const -> std::vector<Pos>
{
    const Board& board = Board::get_board();

    std::vector<Pos> moves;

    for (int y = -1; y <= 1; y++)
    for (int x = -1; x <= 1; x++)
    {
        if (x == 0 && y == 0)
            continue;

        if ((m_pos.x + x < 0 || m_pos.x + x >= board.get_size().x)
        || (m_pos.y + y < 0 || m_pos.y + y >= board.get_size().y))
            continue;

        const Pos next_pos = m_pos + Pos{x, y};

        if (board.get_piece(next_pos)->get_color() != m_color)
            moves.push_back(next_pos);
    }

    // Castling (TODO: Check for check in between)
    if (!m_moved && m_castle_allowed)
    {
        // King side
        Pos next_pos = m_pos + Pos{2, 0};

        if ((board.get_piece(m_pos + Pos{1, 0}) == nullptr)
        && (board.get_piece(next_pos) == nullptr)
        && (board.get_piece(m_pos + Pos{3, 0})->get_type() == "Rook")
        && (!board.get_piece(m_pos + Pos{3, 0})->has_moved()))
            moves.push_back(next_pos);

        // Queen side
        next_pos = m_pos + Pos{-2, 0};

        if ((board.get_piece(m_pos + Pos{-1, 0}) == nullptr)
        && (board.get_piece(m_pos + Pos{-2, 0}) == nullptr)
        && (board.get_piece(m_pos + Pos{-3, 0}) == nullptr)
        && (board.get_piece(m_pos + Pos{-4, 0})->get_type() == "Rook")
        && (!board.get_piece(m_pos + Pos{-4, 0})->has_moved()))
            moves.push_back(next_pos);
    }

    return moves;
}

auto King::get_type() const -> std::string
{
    return "King";
}

auto King::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/king/model.obj"
    };

    return mesh;
}

} // namespace Chess
