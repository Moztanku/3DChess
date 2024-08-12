#pragma once

#include <vector>

#include "Chess/Common.hpp"

#include "Renderer/Mesh.hpp"

namespace Chess
{

class Piece
{
    public:
        Piece(Pos pos, Player color);

        auto move(Pos to) -> void;

        auto has_moved() const -> bool;
        auto get_pos() const -> Pos;
        auto get_color() const -> Player;
        auto draw(const glm::mat4& projView) const -> void;

        virtual auto get_moves() const -> std::vector<Pos> = 0;
        virtual auto get_type() const -> std::string = 0;
    protected:
        Pos m_pos{};
        Player m_color{};
        bool m_moved{false};

        virtual auto get_mesh() const -> const Renderer::Mesh& = 0;
}; // class Piece

class Pawn : public Piece
{
    public:
        Pawn(Pos pos, Player color);

        auto get_moves() const -> std::vector<Pos> override;
        auto get_type() const -> std::string override;
    private:
        auto get_mesh() const -> const Renderer::Mesh& override;
}; // class Pawn

class Bishop : public Piece
{
    public:
        Bishop(Pos pos, Player color);

        auto get_moves() const -> std::vector<Pos> override;
        auto get_type() const -> std::string override;
    private:
        auto get_mesh() const -> const Renderer::Mesh& override;
}; // class Bishop

class Knight : public Piece
{
    public:
        Knight(Pos pos, Player color);

        auto get_moves() const -> std::vector<Pos> override;
        auto get_type() const -> std::string override;
    private:
        auto get_mesh() const -> const Renderer::Mesh& override;
}; // class Knight

class Rook : public Piece
{
    public:
        Rook(Pos pos, Player color);

        auto get_moves() const -> std::vector<Pos> override;
        auto get_type() const -> std::string override;
    private:
        auto get_mesh() const -> const Renderer::Mesh& override;
}; // class Rook

class Queen : public Piece
{
    public:
        Queen(Pos pos, Player color);

        auto get_moves() const -> std::vector<Pos> override;
        auto get_type() const -> std::string override;
    private:
        auto get_mesh() const -> const Renderer::Mesh& override;
}; // class Queen

class King : public Piece
{
    public:
        King(Pos pos, Player color);

        auto get_moves() const -> std::vector<Pos> override;
        auto get_type() const -> std::string override;
    private:
        auto get_mesh() const -> const Renderer::Mesh& override;

        bool m_castle_allowed{true};
}; // class King

} // namespace Chess
