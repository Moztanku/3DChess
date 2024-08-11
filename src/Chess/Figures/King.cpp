#include "Chess/Piece.hpp"

namespace Chess
{

King::King(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto King::get_moves() const -> std::vector<Pos>
{
    return {};
}

auto King::get_type() const -> std::string
{
    return "King";
}

auto King::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/king/model.obj"
    };

    return mesh;
}

} // namespace Chess
