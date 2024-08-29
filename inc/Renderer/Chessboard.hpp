#pragma once

#include <memory>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Renderer/GPU/Shader.hpp"
#include "Renderer/GPU/Texture.hpp"
#include "Renderer/GPU/VertexArray.hpp"
#include "Renderer/GPU/IndexBuffer.hpp"
#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"

#include "Chess/Common.hpp"

namespace Renderer
{

class Chessboard
{
    public:
        Chessboard(Chess::Pos size);

        auto draw(const glm::mat4& MVP) -> void;
    private:
        GPU::Shader m_Shader{"res/shaders/chessboard.vert", "res/shaders/chessboard.frag"};
        GPU::Texture m_Texture{"res/textures/chessboard.png"};

        std::unique_ptr<GPU::VertexArray> m_VAO;
        std::unique_ptr<GPU::VertexBuffer> m_VBO;
}; // class Chessboard

} // namespace Renderer
