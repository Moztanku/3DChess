#pragma once

#include <array>
#include <memory>
#include <string_view>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Renderer/GPU/Texture.hpp"
#include "Renderer/GPU/Shader.hpp"
#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/VertexArray.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"

namespace Renderer
{

namespace
{

// This function draws a single quad that covers the whole screen
auto draw_quad() -> void
{
    static std::unique_ptr<GPU::VertexBuffer> vbo{nullptr};
    static std::unique_ptr<GPU::VertexArray> vao{nullptr};

    if (!vbo || !vao)
    {
        constexpr std::array<float, 12> vertices = {
            -1.f, -1.f,
            1.f, -1.f,
            1.f, 1.f,

            1.f, 1.f,
            -1.f, 1.f,
            -1.f, -1.f
        };

        vbo = std::make_unique<GPU::VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
        vao = std::make_unique<GPU::VertexArray>();

        GPU::VertexBufferLayout layout;
        layout.Push<float>(2);

        vao->AddBuffer(*vbo, layout);

        vbo->Unbind();
        vao->Unbind();
    }

    // Disable depth for background to be always rendered behind everything
    glDisable(GL_DEPTH_TEST);

    vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    vao->Unbind();

    // Enable depth again
    glEnable(GL_DEPTH_TEST);
}

}; // namespace

class Background
{
    public:
        static auto render(const std::string_view background_texture) -> void
        {
            static GPU::Shader shader{"res/shaders/background.vert", "res/shaders/background.frag"};

            if (!m_Texture || m_TexturePath != background_texture)
            {
                m_Texture = std::make_unique<GPU::Texture>(background_texture);
                m_TexturePath = background_texture;
            }

            m_Texture->Bind();
            shader.Bind();

            shader.SetUniform("uTexture", 0);
            shader.SetUniform("uTime", static_cast<float>(glfwGetTime()));

            draw_quad();

            m_Texture->Unbind();
            shader.Unbind();
        }
        
    private:
        inline static std::unique_ptr<GPU::Texture> m_Texture{nullptr};
        inline static std::string m_TexturePath{};
}; // class Background

} // namespace Renderer
