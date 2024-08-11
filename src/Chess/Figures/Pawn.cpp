#include "Chess/Piece.hpp"

namespace Chess
{

Pawn::Pawn(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Pawn::get_moves() const -> std::vector<Pos>
{
    return {};
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
