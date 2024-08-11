#include "Chess/Piece.hpp"

namespace Chess
{

Bishop::Bishop(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Bishop::get_moves() const -> std::vector<Pos>
{
    return {};
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
