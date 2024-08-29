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
    parse_config(config_file, m_Width, m_Height, m_StartingPlayer, m_Pieces);

    m_KingWhite = find_king(m_Pieces, Player::White);
    m_KingBlack = find_king(m_Pieces, Player::Black);

    update();
}

auto Board::getSize() const -> Pos
{
    return Pos{m_Width, m_Height};
}

auto Board::getPieces() const -> const PieceMap&
{
    return m_Pieces;
}

auto Board::getKingPos(const Player color) const -> Pos
{
    return color == Player::White ? m_KingWhite : m_KingBlack;
}

auto Board::getCurrentTurn() const -> Player
{
    if (m_MoveHistory.empty())
        return m_StartingPlayer;
    
    return !m_MoveHistory.back().player;
}

auto Board::getPossibleMoves(const Pos from) const -> std::vector<Move>
{
    auto piece = find_piece(m_Pieces, from);

    if (piece == std::nullopt)
        return {};

    if (piece->color != getCurrentTurn())
        return {};

    std::vector<Move> moves;

    for (const auto& to : m_PossibleMoves.at(from))
        moves.push_back(
            create_move(from, to)
        );

    return moves;
}


auto Board::getPiecesAttackingPos(const Pos pos, const Player color, std::vector<Pos>& attackers) const -> void
{
    for (const auto& [piece_pos, piece] : m_Pieces)
    {
        if (piece.color != color)
            continue;

        const auto moves = get_moves(piece_pos);

        if (std::find(moves.begin(), moves.end(), pos) != moves.end())
            attackers.push_back(piece_pos);
    }
}

auto Board::checkIfAttackingPos(const Pos pos, const Player color) const -> bool
{
    for (const auto& [piece_pos, piece] : m_Pieces)
    {
        if (piece.color != color)
            continue;

        const auto moves = get_moves(piece_pos);

        if (std::find(moves.begin(), moves.end(), pos) != moves.end())
            return true;
    }

    return false;
}

auto Board::executeMove(const Move& move) -> void
{

    execute(move);
    update();
}

auto Board::undoMove() -> Move
{
    auto move = undo();
    update();

    return move;
}

auto Board::getCurrentGameState() const -> Controller::GameState
{
    if (m_MoveHistory.empty())
        return Controller::GameState::Playing;

    const auto& last_move = m_MoveHistory.back();

    if (last_move.isType(Move::Type::Checkmate))
        return last_move.player == Player::White ?
            Controller::GameState::WhiteWin : Controller::GameState::BlackWin;

    if (last_move.isType(Move::Type::Stalemate))
        return Controller::GameState::Draw;

    return Controller::GameState::Playing;
}

auto Board::reset() -> void
{
    while(!m_MoveHistory.empty())
        undo();

    update();
}

// Private

auto Board::update() -> void
{
    if (m_MoveHistory.empty())
        return calculate_possible_moves_initial();

    calculate_possible_moves(m_MoveHistory.back());

    auto& last_move = m_MoveHistory.back();

    const std::vector<Pos>& moves = m_PossibleMoves.at(last_move.to);

    // Add info about check to the move
    if (std::find(moves.begin(), moves.end(),
        getKingPos(!last_move.player)) != moves.end())
    {
        last_move.type |= static_cast<uint>(Move::Type::Check);
    }

    // Add info about checkmate/stalemate to the move
    Player current = getCurrentTurn();
    bool no_moves = true;

    for (const auto& [pos, moves] : m_PossibleMoves)
        if (m_Pieces.at(pos).color == current && !moves.empty())
        {
            no_moves = false;
            break;
        }

    if (no_moves)
    {
        if (last_move.isType(Move::Type::Check))
            last_move.type |= static_cast<uint>(Move::Type::Checkmate);
        else
            last_move.type |= static_cast<uint>(Move::Type::Stalemate);
    }
}

auto Board::execute(const Move& move) -> void
{
    m_MoveHistory.push_back(move);

    m_Pieces[move.to] = m_Pieces[move.from];
    m_Pieces[move.to].moved = true;

    if (m_Pieces[move.to].type == Piece::Type::King)
    {
        if (m_Pieces[move.to].color == Player::White)
            m_KingWhite = move.to;
        else
            m_KingBlack = move.to;
    }

    m_Pieces.erase(move.from);

    auto it = move.specialMoveInfo.data();

    if (move.isType(Move::Type::Capture))
    {
        const Pos cap_pos = *reinterpret_cast<const Pos*>(it);
        it += sizeof(cap_pos);

        const Piece cap_piece = *reinterpret_cast<const Piece*>(it);
        it += sizeof(cap_piece);

        if (cap_pos != move.to)
            m_Pieces.erase(cap_pos);
    }

    if (move.isType(Move::Type::Promotion))
    {
        const Piece::Type promoted = *reinterpret_cast<const Piece::Type*>(it);
        it += sizeof(promoted);

        m_Pieces[move.to].type = promoted;
    }

    if (move.isType(Move::Type::Castling))
    {
        const Pos rook_from = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_from);

        const Pos rook_to = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_to);

        m_Pieces[rook_to] = m_Pieces[rook_from];
        m_Pieces[rook_to].moved = true;

        m_Pieces.erase(rook_from);
    }
}

auto Board::undo() -> Move
{
    if (m_MoveHistory.empty())
    {
        std::cerr << "No moves to undo!" << std::endl;
        return Move{};
    }

    const Move move = m_MoveHistory.back();
    m_MoveHistory.pop_back();

    m_Pieces[move.from] = m_Pieces[move.to];
    m_Pieces.erase(move.to);

    if (m_Pieces[move.from].type == Piece::Type::King)
    {
        if (m_Pieces[move.from].color == Player::White)
            m_KingWhite = move.from;
        else
            m_KingBlack = move.from;
    }

    auto it = move.specialMoveInfo.data();

    if (move.isType(Move::Type::Capture))
    {
        const Pos cap_pos = *reinterpret_cast<const Pos*>(it);
        it += sizeof(cap_pos);

        const Piece cap_piece = *reinterpret_cast<const Piece*>(it);
        it += sizeof(cap_piece);

        m_Pieces[cap_pos] = cap_piece;
    }

    if (move.isType(Move::Type::Promotion))
    {
        m_Pieces[move.from].type = Piece::Type::Pawn;
    }

    if (move.isType(Move::Type::Castling))
    {
        const Pos rook_from = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_from);

        const Pos rook_to = *reinterpret_cast<const Pos*>(it);
        it += sizeof(rook_to);

        m_Pieces[rook_from] = m_Pieces[rook_to];
        m_Pieces[rook_from].moved = false;

        // Check in case if kings initial position is 1 square away from rook
        if (rook_to != move.from)
            m_Pieces.erase(rook_to);
    }

    if (move.isType(Move::Type::FirstMove))
    {
        m_Pieces[move.from].moved = false;
    }

    return move;
}

auto Board::create_move(const Pos from, const Pos to, const std::optional<Piece::Type> promotion) const -> Move
{
    if (m_Pieces.find(from) == m_Pieces.end())
        return Move{};

    const Piece piece = m_Pieces.at(from);

    uint8_t type {static_cast<uint8_t>(Move::Type::Empty)};
    Move::SpecialMoveInfo sm_info{};

    auto captured = find_piece(m_Pieces, to);

    // En passant
    if (piece.type == Piece::Type::Pawn && captured == std::nullopt && to.x != from.x)
    {
        type |= static_cast<uint>(Move::Type::EnPassant);

        captured = find_piece(m_Pieces, Pos{to.x, from.y});
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
        if ((piece.color == Player::White && to.y == getSize().y - 1) || (piece.color == Player::Black && to.y == 0))
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

            while(find_piece(m_Pieces, rook_from) == std::nullopt)
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
        .specialMoveInfo = sm_info
    };
}

auto Board::calculate_possible_moves(const Move& /* move */) -> void
{
    // TODO: Only update the moves that are affected by the last move
    calculate_possible_moves_initial();
}

auto Board::calculate_possible_moves_initial() -> void
{
    m_PossibleMoves.clear();

    // Get all moves a piece can make
    for (const auto& [pos, piece] : m_Pieces)
        m_PossibleMoves[pos] = get_moves(pos);

    // Filter out moves that would put the king in check
    for (auto& [pos, moves] : m_PossibleMoves)
    {
        const Piece piece = m_Pieces.at(pos);

        auto it = moves.begin();

        while (it != moves.end())
        {
            const Move move = create_move(pos, *it);

            execute(move);

            const Pos king_pos = getKingPos(piece.color);

            if (checkIfAttackingPos(king_pos, !piece.color))
                it = moves.erase(it);
            else
                ++it;

            undo();
        }
    }
}

} // namespace Chess
