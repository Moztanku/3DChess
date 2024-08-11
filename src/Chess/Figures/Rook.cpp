#include "Chess/Piece.hpp"

namespace Chess
{

Rook::Rook(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Rook::get_moves() const -> std::vector<Pos>
{
    return {};
}

auto Rook::get_type() const -> std::string
{
    return "Rook";
}

auto Rook::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/rook/model.obj"
    };

    return mesh;
}

} // namespace Chess
