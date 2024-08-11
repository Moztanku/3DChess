#include "Chess/Piece.hpp"

namespace Chess
{

Knight::Knight(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Knight::get_moves() const -> std::vector<Pos>
{
    return {};
}

auto Knight::get_type() const -> std::string
{
    return "Knight";
}

auto Knight::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/horse/model.obj"
    };

    return mesh;
}

} // namespace Chess
