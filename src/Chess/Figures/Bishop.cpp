#include "Chess/Piece.hpp"

#include "Chess/Board.hpp"

namespace Chess
{

Bishop::Bishop(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Bishop::get_moves() const -> std::vector<Pos>
{
    const Board& board = Board::get_board();
    std::vector<Pos> moves;

    constexpr std::array<Pos, 4> directions = {
        Pos{-1, 1},     Pos{1, 1},

        Pos{-1, -1},    Pos{1, -1}
    };

    for (const Pos& direction : directions)
    {
        Pos next = m_pos;
        while (true)
        {
            next = next + direction;

            if ((next.x < 0 || next.x >= board.get_size().x)
            || (next.y < 0 || next.y >= board.get_size().y))
                break;

            if (board.get_piece(next) == nullptr)
            {
                moves.push_back(next);
            }
            else
            {
                if (board.get_piece(next)->get_color() != m_color)
                    moves.push_back(next);

                break;
            }
        }
    }

    return moves;
}

auto Bishop::get_type() const -> std::string
{
    return "Bishop";
}

auto Bishop::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/bishop/model.obj"
    };

    return mesh;
}

} // namespace Chess
