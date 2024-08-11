#include "Chess/Piece.hpp"

namespace Chess
{

Queen::Queen(Pos pos, Player color)
    : Piece{pos, color}
{
}

auto Queen::get_moves() const -> std::vector<Pos>
{
    return {};
}

auto Queen::get_type() const -> std::string
{
    return "Queen";
}

auto Queen::get_mesh() const -> const Renderer::Mesh&
{
    static const Renderer::Mesh mesh{
        "res/models/queen/model.obj"
    };

    return mesh;
}

} // namespace Chess
