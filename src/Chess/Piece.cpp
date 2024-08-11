#include "Chess/Piece.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/GPU/Shader.hpp"

namespace Chess
{

Piece::Piece(Pos pos, Player color)
    : m_pos{pos}
    , m_color{color}
{
}

auto Piece::move(Pos to) -> void
{
    m_pos = to;
    m_moved = true;
}

auto Piece::get_pos() const -> Pos
{
    return m_pos;
}

auto Piece::get_color() const -> Player
{
    return m_color;
}

auto Piece::draw(const glm::mat4& projView) const -> void
{
    static Renderer::GPU::Shader piece_shader{
        "res/shaders/piece.vert",
        "res/shaders/piece.frag"
    };

    glm::mat4 model = glm::mat4(1.0f);

    constexpr float scale = 12.f;
    constexpr float border = 32.f / scale;
    constexpr float square = 64.f / scale;

    model = glm::scale(model, glm::vec3(scale, scale, scale));

    const float x = m_pos.x * square + 2 * border;
    const float y = m_pos.y * square + 2 * border;

    model = glm::translate(model, glm::vec3(x, y, 0.f));

    if (m_color == Player::White)
        model = glm::rotate(
            model, glm::radians(180.f),
            glm::vec3(0.f, 0.f, 1.f)
        );

    piece_shader.Bind();

    const float color = m_color == Player::White ? 0.9f : 0.2f;

    piece_shader.SetUniform("uColor", color, color, color);
    piece_shader.SetUniformM("uModel", model);
    piece_shader.SetUniformM("uMVP", projView * model);

    get_mesh().Draw();

    piece_shader.Unbind();
}

} // namespace Chess
