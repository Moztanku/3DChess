#include "Chess/Board.hpp"

#include <array>
#include <optional>

#include "Chess/Common.hpp"

namespace
{

auto find_piece(const Chess::Board::PieceMap& pieces, const Chess::Pos pos) -> std::optional<Chess::Piece>
{
    auto it = pieces.find(pos);

    if (it == pieces.end())
        return std::nullopt;

    return it->second;
}

auto append_moves_in_direction(
    const Chess::Board::PieceMap& pieces,
    const Chess::Pos from,
    const Chess::Pos dir,
    const Chess::Pos size,
    std::vector<Chess::Pos>& moves) -> void
{
    const Chess::Player color = pieces.at(from).color;

    Chess::Pos next = from + dir;

    while (Chess::in_bounds(next, size))
    {
        auto piece = find_piece(pieces, next);

        if (piece == std::nullopt)
        {
            moves.push_back(next);
        }
        else
        {
            if (piece->color != color)
                moves.push_back(next);

            break;
        }

        next += dir;
    }
}

} // namespace

namespace Chess
{

auto Board::get_moves(const Pos from) const -> std::vector<Pos>
{
    using Type = Piece::Type;

    const auto piece = m_pieces.find(from);

    if (piece == m_pieces.end())
        return {};

    const auto [pos, pic] = *piece;

    return pic.type == Type::Pawn   ? get_moves_by_type<Type::Pawn>(from, pic.color)
        : pic.type == Type::Knight  ? get_moves_by_type<Type::Knight>(from, pic.color)
        : pic.type == Type::Bishop  ? get_moves_by_type<Type::Bishop>(from, pic.color)
        : pic.type == Type::Rook    ? get_moves_by_type<Type::Rook>(from, pic.color)
        : pic.type == Type::Queen   ? get_moves_by_type<Type::Queen>(from, pic.color)
        : pic.type == Type::King    ? get_moves_by_type<Type::King>(from, pic.color)
        : std::vector<Pos>{};
}

template<>
auto Board::get_moves_by_type<Piece::Type::Pawn>(const Pos from, const Player color) const -> std::vector<Pos>
{
    std::vector<Pos> moves;

    const int forward =
        color == Player::White ? 1 : -1;

    // Move forward
    Pos next_pos = from + Pos{0, forward};

    if (find_piece(m_pieces, next_pos) == std::nullopt)
    {
        moves.push_back(next_pos);

        // Move two squares forward
        if (!m_pieces.at(from).moved)
        {
            next_pos = from + Pos{0, forward * 2};

            if (find_piece(m_pieces, next_pos) == std::nullopt
            && in_bounds(next_pos, get_size()))
                moves.push_back(next_pos);
        }
    }

    // Capture
    for (int x = -1; x <= 1; x += 2)
    {
        next_pos = from + Pos{x, forward};
        auto piece = find_piece(m_pieces, next_pos);

        if (piece != std::nullopt && piece->color != color)
        {
            moves.push_back(next_pos);
        }
    }

    // En passant
    if (!m_move_history.empty())
    {
        const Move& last_move = m_move_history.back();

        if (last_move.piece == Piece::Type::Pawn
            && last_move.is_type(Move::Type::FirstMove)
            && std::abs(last_move.from.y - last_move.to.y) == 2
            && std::abs(last_move.to.x - from.x) == 1
            && last_move.to.y == from.y)
            moves.push_back(last_move.to + Pos{0, forward});
    }

    return moves;
}

template<>
auto Board::get_moves_by_type<Piece::Type::Knight>(const Pos from, const Player color) const -> std::vector<Pos>
{
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
        Pos next = from + dir;

        if (!in_bounds(next, get_size()))
            continue;

        auto piece = find_piece(m_pieces, next);

        if (piece == std::nullopt || piece->color != color)
            moves.push_back(next);
    }

    return moves;
}

template<>
auto Board::get_moves_by_type<Piece::Type::Bishop>(const Pos from, const Player /* color */) const -> std::vector<Pos>
{
    std::vector<Pos> moves;

    constexpr std::array<Pos, 4> directions = {
        Pos{-1, 1},     Pos{1, 1},

        Pos{-1, -1},    Pos{1, -1}
    };

    for (const Pos dir : directions)
        append_moves_in_direction(
            m_pieces,
            from,
            dir,
            get_size(),
            moves);

    return moves;
}

template<>
auto Board::get_moves_by_type<Piece::Type::Rook>(const Pos from, const Player /* color */) const -> std::vector<Pos>
{
    std::vector<Pos> moves;

    constexpr std::array<Pos, 4> directions = {
                Pos{0, 1},
        Pos{-1, 0},     Pos{1, 0},
                Pos{0, -1}
    };

    for (const Pos dir : directions)
        append_moves_in_direction(
            m_pieces,
            from,
            dir,
            get_size(),
            moves);

    return moves;
}

template<>
auto Board::get_moves_by_type<Piece::Type::Queen>(const Pos from, const Player /* color */) const -> std::vector<Pos>
{
    std::vector<Pos> moves;

    constexpr std::array<Pos, 8> directions = {
            Pos{-1, 1},     Pos{0, 1},      Pos{1, 1},
            Pos{-1, 0},                     Pos{1, 0},
            Pos{-1, -1},    Pos{0, -1},     Pos{1, -1}
    };

    for (const Pos dir : directions)
        append_moves_in_direction(
            m_pieces,
            from,
            dir,
            get_size(),
            moves);

    return moves;
}

template<>
auto Board::get_moves_by_type<Piece::Type::King>(const Pos from, const Player color) const -> std::vector<Pos>
{
    std::vector<Pos> moves;

    for (int y = -1; y <= 1; y++)
    for (int x = -1; x <= 1; x++)
    {
        if (x == 0 && y == 0)
            continue;

        const Pos next_pos = from + Pos{x, y};

        if (!in_bounds(next_pos, get_size()))
            continue;

        auto piece = find_piece(m_pieces, next_pos);

        if (piece == std::nullopt || piece->color != color)
            moves.push_back(next_pos);
    }

    // Castling
    if (!m_pieces.at(from).moved)
    {
        constexpr std::array<Pos, 2> directions = {
            Pos{-1, 0}, Pos{1, 0}
        };

        for (const Move& move : m_move_history)
            if (move.is_type(Move::Type::Check) && move.player != color)
                return moves;

        for (const Pos dir : directions)
        {
            Pos next = from + dir;

            while (in_bounds(next, get_size()))
            {
                auto piece = find_piece(m_pieces, next);

                if (piece == std::nullopt)
                {
                    next += dir;
                    continue;
                }

                if (piece->type == Piece::Type::Rook && !piece->moved)
                {
                    check_if_attacking_pos(
                        from + dir, color == Player::White ? Player::Black : Player::White);
                    
                    moves.push_back(from + dir * 2);
                }

                // Break if we found a piece that is not a rook, or a rook that has moved
                break;
            }
        }
    }

    return moves;
}

} // namespace Chess
