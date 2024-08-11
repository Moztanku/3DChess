#pragma once

#include "Chess/Board.hpp"

namespace Chess
{

class Game
{
    public:
        enum class State
        {
            TurnWhite, TurnBlack,
            WhiteWon, BlackWon,
            Stalemate
        };

        Game(const Board& board) : m_state{State::TurnWhite}, m_board{board} {}

        auto get_state() const -> State { return m_state; }
        auto next_state() -> void;
    private:
        State m_state;
        const Board& m_board;
};

}; // namespace Chess
