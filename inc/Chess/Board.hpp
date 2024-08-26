#pragma once

#include <string_view>
#include <unordered_map>

#include <glm/vec2.hpp>

#include "Chess/Common.hpp"
#include "Chess/Piece.hpp"
#include "Chess/Move.hpp"
#include "Controller/GameState.hpp"

namespace Chess
{

class Board
{
    public:
        using PieceMap = std::unordered_map
            <Pos, Piece, PosKeyFuncs>;
        using MoveMap = std::unordered_map
            <Pos, std::vector<Pos>, PosKeyFuncs>;
        using MoveList = std::vector<Move>;
            
        Board(const std::string_view layout_file);

        auto get_size() const -> Pos;
        auto get_pieces() const -> const PieceMap&;
        auto get_king_pos(Player color) const -> Pos;
        auto get_current_turn() const -> Player;
        auto get_possible_moves(const Pos from) const -> std::vector<Move>;
        auto get_pieces_atacking_pos(const Pos pos, const Player color, std::vector<Pos>& attackers) const -> void;
        auto check_if_attacking_pos(const Pos pos, const Player color) const -> bool;
        auto get_current_game_state() const -> Controller::GameState;

        auto execute_move(const Move& move) -> void;
        auto undo_move() -> Move;
        auto reset() -> void;
    private:
        PieceMap m_pieces;
        MoveMap m_possible_moves;
        MoveList m_move_history;

        Pos m_KingWhitePos{-1,-1};
        Pos m_KingBlackPos{-1,-1};

        // // TODO: Change chessboard class into a Mesh class
        // Renderer::Chessboard* m_chessboard;
        
        // Board dimensions, pseudo-constants as they are set in the constructor
        Player m_starting_player{Player::White};
        uint m_width;
        uint m_height;

        auto update() -> void;

        auto execute(const Move& move) -> void;
        auto undo() -> Move;

        auto create_move(
            const Pos from,
            const Pos to,
            const std::optional<Piece::Type> promotion = std::nullopt) const -> Move;

        auto calculate_possible_moves(const Move& move) -> void;
        auto calculate_possible_moves_initial() -> void;

        // Defined in Chess/Figures.cpp
        auto get_moves(const Pos from) const -> std::vector<Pos>;

        template <Piece::Type type>
        auto get_moves_by_type(const Pos from, const Player color) const -> std::vector<Pos>;
}; // class Board

template<>
auto Board::get_moves_by_type<Piece::Type::Pawn>(const Pos, const Player color) const -> std::vector<Pos>;

template<>
auto Board::get_moves_by_type<Piece::Type::Knight>(const Pos, const Player color) const -> std::vector<Pos>;

template<>
auto Board::get_moves_by_type<Piece::Type::Bishop>(const Pos, const Player color) const -> std::vector<Pos>;

template<>
auto Board::get_moves_by_type<Piece::Type::Rook>(const Pos, const Player color) const -> std::vector<Pos>;

template<>
auto Board::get_moves_by_type<Piece::Type::Queen>(const Pos, const Player color) const -> std::vector<Pos>;

template<>
auto Board::get_moves_by_type<Piece::Type::King>(const Pos, const Player color) const -> std::vector<Pos>;

} // namespace Chess
