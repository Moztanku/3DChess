#include "Renderer/UIBox.hpp"

#include <array>

#include "Renderer/GPU/Shader.hpp"
#include "Renderer/GPU/VertexArray.hpp"
#include "Renderer/GPU/IndexBuffer.hpp"
#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"

namespace
{

auto init_vbo() -> Renderer::GPU::VertexBuffer
{
    using namespace Renderer::GPU;

    constexpr std::array<float, 12> vertices = {
        -1.f,   -1.f,
         1.f,   -1.f,
         1.f,    1.f,

         1.f,    1.f,
        -1.f,    1.f,
        -1.f,   -1.f
    };

    return VertexBuffer{vertices.data(), vertices.size() * sizeof(float)};
}

auto init_vao(const Renderer::GPU::VertexBuffer& vbo) -> Renderer::GPU::VertexArray
{
    using namespace Renderer::GPU;

    VertexBufferLayout layout;
    layout.Push<float>(2);

    VertexArray vao;
    vao.AddBuffer(vbo, layout);

    return vao;
}

} // namespace


namespace Renderer
{

UIBox::UIBox(const std::string_view tex_path) : m_Texture{tex_path}
{}

auto UIBox::draw() const -> void
{
    draw(Settings{});
}

auto UIBox::draw(const Settings& settings) const -> void
{
    static GPU::Shader shader {
        "res/shaders/ui_box.vert",
        "res/shaders/ui_box.frag"
    };

    static const GPU::VertexBuffer vbo = init_vbo();
    static const GPU::VertexArray vao = init_vao(vbo);

    shader.Bind();

    shader.SetUniform("uPos", settings.position.x, settings.position.y);
    shader.SetUniform("uSize", settings.size.x, settings.size.y);
    shader.SetUniform("uAlpha", settings.alpha);
    shader.SetUniform("uColor", settings.color.r, settings.color.g, settings.color.b);

    shader.SetUniform("uTexture", 0);

    m_Texture.Bind();

    vao.Bind();

    glDisable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_DEPTH_TEST);

    vao.Unbind();

    m_Texture.Unbind();

    shader.Unbind();
}

} // namespace Renderer
