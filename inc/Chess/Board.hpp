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

        auto getSize() const -> Pos;
        auto getPieces() const -> const PieceMap&;
        auto getKingPos(Player color) const -> Pos;
        auto getCurrentTurn() const -> Player;
        auto getPossibleMoves(const Pos from) const -> std::vector<Move>;
        auto getPiecesAttackingPos(const Pos pos, const Player color, std::vector<Pos>& attackers) const -> void;
        auto getCurrentGameState() const -> Controller::GameState;

        auto checkIfAttackingPos(const Pos pos, const Player color) const -> bool;

        auto executeMove(const Move& move) -> void;
        auto undoMove() -> Move;
        auto reset() -> void;
    private:
        PieceMap m_Pieces;
        MoveMap m_PossibleMoves;
        MoveList m_MoveHistory;

        Pos m_KingWhite{-1,-1};
        Pos m_KingBlack{-1,-1};
        
        // Board dimensions, pseudo-constants as they are set in the constructor
        Player m_StartingPlayer{Player::White};
        uint m_Width;
        uint m_Height;

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
