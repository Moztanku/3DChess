#include "Chess/Board.hpp"

#include <fstream>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Mesh.hpp"

namespace
{

auto parse_layout(const std::vector<std::string>& layout,
    const uint width,
    const uint height,
    Chess::Board::PieceMap& piece_map) -> void
{
    for (uint y = 0; y < height; y++)
    {
        uint first = layout[y].find_first_not_of(' ');

        for (uint x = 0; x < width; x++)
        {
            if (layout[y][first] == '.')
            {
                first = layout[y].find_first_not_of(' ', first + 1);
                continue;
            }

            // color
            const char c = layout[y][first];
            // piece type
            const char p = layout[y][first + 1];

            first = layout[y].find_first_not_of(' ', first + 2);

            const Chess::Pos pos{
                x, height - (y + 1)};
            const Chess::Player color =
                c == 'W' ? Chess::Player::White : Chess::Player::Black;
                
            Chess::Piece piece{
                    .color = color,
                    .type = static_cast<Chess::Piece::Type>(p - '0')};

            piece_map[pos] = piece;
        }
    }
}

auto parse_config(
    const std::string_view config_file,
    uint& width,
    uint& height,
    Chess::Player& starting_player,
    Chess::Board::PieceMap& piece_map) -> void
{
    std::fstream file{config_file.data(), std::ios::in};

    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << config_file << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        // Split the line into words separated by spaces
        std::vector<std::string_view> words{};

        size_t start = 0;
        size_t end = 0;

        while (end != std::string::npos)
        {
            end = line.find(' ', start);

            words.push_back(
                std::string_view{line}.substr(start, end - start)
            );

            start = end + 1;
        }

        // Parse the words
        if (words[0] == "size")
        {
            width = std::stol(std::string{words[1]});
            height = std::stol(std::string{words[2]});
        }
        else if (words[0] == "player")
        {
            starting_player = words[1] == "white" ? Chess::Player::White : Chess::Player::Black;
        }
        else if (words[0] == "layout")
        {
            std::vector<std::string> layout{};

            for (size_t i = 0; i < height; i++)
            {
                std::getline(file, line);

                layout.push_back(line);
            }

            parse_layout(
                layout,
                width,
                height,
                piece_map);

            break;
        }
        else
        {
            std::cerr << "Unknown command: " << line << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    file.close();
}

auto find_king(const Chess::Board::PieceMap& pieces, const Chess::Player color) -> Chess::Pos
{
    Chess::Pos king_pos{-1, -1};

    for (const auto& [pos, piece] : pieces)
    {
        if (piece.type == Chess::Piece::Type::King && piece.color == color)
        {
            if (king_pos == Chess::Pos{-1, -1})
                king_pos = pos;
            else
            {
                std::cerr << "Multiple kings found for color " << static_cast<int>(color) << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }

    if (king_pos == Chess::Pos{-1, -1})
    {
        std::cerr << "No king found for color " << static_cast<int>(color) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return king_pos;
}

auto find_piece(const Chess::Board::PieceMap& pieces, const Chess::Pos pos) -> std::optional<Chess::Piece>
{
    auto it = pieces.find(pos);

    if (it == pieces.end())
        return std::nullopt;

    return it->second;
}

template <typename T>
auto append_data(std::vector<std::byte>& data, const T& value) -> void
{
    const std::byte* bytes = reinterpret_cast<const std::byte*>(&value);

    data.insert(data.end(), bytes, bytes + sizeof(T));
}

} // namespace

namespace Chess
{

// Public

Board::Board(const std::string_view config_file)
{
    parse_config(config_file, m_width, m_height, m_starting_player, m_pieces);

    m_KingWhitePos = find_king(m_pieces, Player::White);
    m_KingBlackPos = find_king(m_pieces, Player::Black);

    update();
}

auto Board::get_size() const -> Pos
{
    return Pos{m_width, m_height};
}

auto Board::get_pieces() const -> const PieceMap&
{
    return m_pieces;
}

auto Board::get_king_pos(const Player color) const -> Pos
{
    return color == Player::White ? m_KingWhitePos : m_KingBlackPos;
}

auto Board::get_current_turn() const -> Player
{
    if (m_move_history.empty())
        return m_starting_player;
    
    return !m_move_history.back().player;
}

auto Board::get_possible_moves(const Pos from) const -> std::vector<Move>
{
    auto piece = find_piece(m_pieces, from);

    if (piece == std::nullopt)
        return {};

    if (piece->color != get_current_turn())
        return {};

    std::vector<Move> moves;

    for (const auto& to : m_possible_moves.at(from))
        moves.push_back(
            create_move(from, to)
        );

    return moves;
}


auto Board::get_pieces_atacking_pos(const Pos pos, const Player color, std::vector<Pos>& attackers) const -> void
{
    for (const auto& [piece_pos, piece] : m_pieces)
    {
        if (piece.color != color)
            continue;

        const auto moves = get_moves(piece_pos);

        if (std::find(moves.begin(), moves.end(), pos) != moves.end())
            attackers.push_back(piece_pos);
    }
}

auto Board::check_if_attacking_pos(const Pos pos, const Player color) const -> bool
{
    for (const auto& [piece_pos, piece] : m_pieces)
    {
        if (piece.color != color)
            continue;

        const auto moves = get_moves(piece_pos);

        if (std::find(moves.begin(), moves.end(), pos) != moves.end())
            return true;
    }

    return false;
}

auto Board::execute_move(const Move& move) -> void
{

    execute(move);
    update();
}

auto Board::undo_move() -> Move
{
    auto move = undo();
    update();

    return move;
}

auto Board::get_current_game_state() const -> Controller::GameState
{
    if (m_move_history.empty())
        return Controller::GameState::Playing;

    const auto& last_move = m_move_history.back();

    if (last_move.is_type(Move::Type::Checkmate))
        return last_move.player == Player::White ?
            Controller::GameState::WhiteWin : Controller::GameState::BlackWin;

    if (last_move.is_type(Move::Type::Stalemate))
        return Controller::GameState::Draw;

    return Controller::GameState::Playing;
}

auto Board::reset() -> void
{
    while(!m_move_history.empty())
        undo();

    update();
}

// Private

auto Board::update() -> void
{
    if (m_move_history.empty())
        return calculate_possible_moves_initial();

    calculate_possible_moves(m_move_history.back());

    auto& last_move = m_move_history.back();

    const std::vector<Pos>& moves = m_possible_moves.at(last_move.to);

    // Add info about check to the move
    if (std::find(moves.begin(), moves.end(),
        get_king_pos(!last_move.player)) != moves.end())
    {
        last_move.type |= static_cast<uint>(Move::Type::Check);
    }

    // Add info about checkmate/stalemate to the move
    Player current = get_current_turn();
    bool no_moves = true;

    for (const auto& [pos, moves] : m_possible_moves)
        if (m_pieces.at(pos).color == current && !moves.empty())
        {
            no_moves = false;
            break;
        }

    if (no_moves)
    {
        if (last_move.is_type(Move::Type::Check))
            last_move.type |= static_cast<uint>(Move::Type::Checkmate);
        else
            last_move.type |= static_cast<uint>(Move::Type::Stalemate);
    }
}

auto Board::execute(const Move& move) -> void
{
    m_move_history.push_back(move);

    m_pieces[move.to] = m_pieces[move.from];
    m_pieces[move.to].moved = true;

    if (m_pieces[move.to].type == Piece::Type::King)
    {
        if (m_pieces[move.to].color == Player::White)
            m_KingWhitePos = move.to;
        else
            m_KingBlackPos = move.to;
    }

    m_pieces.erase(move.from);

    auto it = move.special_move_info.data();

    if (move.is_type(Move::Type::Capture))
    {
        const Pos cap_pos = *reinterpret_cast<const Pos*>(it);
        it += sizeof(cap_pos);

        const Piece cap_piece = *reinterpret_cast<const Piece*>(it);
        it += sizeof(cap_piece);

        if (cap_pos != move.to)
            m_pieces.erase(cap_pos);
    }

    if (move.is_type(Move::Type::Promotion))
    {
        const Piece::Type promoted = *reinterpret_cast<const Piece::Type*>(it);
        it += sizeof(promoted);

        m_pieces[move.to].type = promoted;
    }

    if (move.is_type(Move::Type::Castling))
    {
        const Pos rook_from = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_from);

        const Pos rook_to = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_to);

        m_pieces[rook_to] = m_pieces[rook_from];
        m_pieces[rook_to].moved = true;

        m_pieces.erase(rook_from);
    }
}

auto Board::undo() -> Move
{
    if (m_move_history.empty())
    {
        std::cerr << "No moves to undo!" << std::endl;
        return Move{};
    }

    const Move move = m_move_history.back();
    m_move_history.pop_back();

    m_pieces[move.from] = m_pieces[move.to];
    m_pieces.erase(move.to);

    if (m_pieces[move.from].type == Piece::Type::King)
    {
        if (m_pieces[move.from].color == Player::White)
            m_KingWhitePos = move.from;
        else
            m_KingBlackPos = move.from;
    }

    auto it = move.special_move_info.data();

    if (move.is_type(Move::Type::Capture))
    {
        const Pos cap_pos = *reinterpret_cast<const Pos*>(it);
        it += sizeof(cap_pos);

        const Piece cap_piece = *reinterpret_cast<const Piece*>(it);
        it += sizeof(cap_piece);

        m_pieces[cap_pos] = cap_piece;
    }

    if (move.is_type(Move::Type::Promotion))
    {
        m_pieces[move.from].type = Piece::Type::Pawn;
    }

    if (move.is_type(Move::Type::Castling))
    {
        const Pos rook_from = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_from);

        const Pos rook_to = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_to);

        m_pieces[rook_from] = m_pieces[rook_to];
        m_pieces[rook_from].moved = false;

        // Check in case if kings initial position is 1 square away from rook
        if (rook_to != move.from)
            m_pieces.erase(rook_to);
    }

    if (move.is_type(Move::Type::FirstMove))
    {
        m_pieces[move.from].moved = false;
    }

    return move;
}

auto Board::create_move(const Pos from, const Pos to, const std::optional<Piece::Type> promotion) const -> Move
{
    if (m_pieces.find(from) == m_pieces.end())
        return Move{};

    const Piece piece = m_pieces.at(from);

    uint8_t type {static_cast<uint8_t>(Move::Type::Empty)};
    Move::SpecialMoveInfo sm_info{};

    auto captured = find_piece(m_pieces, to);

    // En passant
    if (piece.type == Piece::Type::Pawn && captured == std::nullopt && to.x != from.x)
    {
        type |= static_cast<uint>(Move::Type::EnPassant);

        captured = find_piece(m_pieces, Pos{to.x, from.y});
    }

    // Capture
    if (captured != std::nullopt)
    {
        type |= static_cast<uint>(Move::Type::Capture);

        if (type & static_cast<uint>(Move::Type::EnPassant))
            append_data(sm_info, Pos{to.x, from.y});
        else
            append_data(sm_info, to);
                    
        append_data(sm_info, captured.value());
    }

    // Promotion
    if (piece.type == Piece::Type::Pawn)
    {
        if ((piece.color == Player::White && to.y == get_size().y - 1) || (piece.color == Player::Black && to.y == 0))
        {
            type |= static_cast<uint>(Move::Type::Promotion);

            append_data(sm_info, static_cast<Piece::Type>(promotion.value_or(Piece::Type::Queen)));
        }
    }

    // Castling
    if (piece.type == Piece::Type::King)
    {
        if (std::abs(from.x - to.x) == 2)
        {
            type |= static_cast<uint>(Move::Type::Castling);

            const int dir = to.x - from.x > 0 ? 1 : -1;

            Pos rook_to = to - Pos{dir, 0};
            Pos rook_from = to;

            while(find_piece(m_pieces, rook_from) == std::nullopt)
                rook_from += Pos{dir, 0};

            append_data(sm_info, rook_from);
            append_data(sm_info, rook_to);
        }
    }

    // First move
    if (!piece.moved)
    {
        type |= static_cast<uint>(Move::Type::FirstMove);
    }

    return Move{
        .player = piece.color,
        .from = from,
        .to = to,
        .piece = piece.type,
        .type = type,
        .special_move_info = sm_info
    };
}

auto Board::calculate_possible_moves(const Move& /* move */) -> void
{
    // TODO: Only update the moves that are affected by the last move
    calculate_possible_moves_initial();
}

auto Board::calculate_possible_moves_initial() -> void
{
    m_possible_moves.clear();

    // Get all moves a piece can make
    for (const auto& [pos, piece] : m_pieces)
        m_possible_moves[pos] = get_moves(pos);

    // Filter out moves that would put the king in check
    for (auto& [pos, moves] : m_possible_moves)
    {
        const Piece piece = m_pieces.at(pos);

        auto it = moves.begin();

        while (it != moves.end())
        {
            const Move move = create_move(pos, *it);

            execute(move);

            const Pos king_pos = get_king_pos(piece.color);

            if (check_if_attacking_pos(king_pos, !piece.color))
                it = moves.erase(it);
            else
                ++it;

            undo();
        }
    }
}

} // namespace Chess
