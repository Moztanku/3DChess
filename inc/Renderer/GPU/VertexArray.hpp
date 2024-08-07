/**
 * @file VertexArray.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Vertex Array Object (VAO), describes how to interpret vertex data
 * @version 0.1
 * @date 2024-01-05
 * @see VertexBuffer.hpp VertexBufferLayout.hpp
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"

namespace Renderer::GPU {

class VertexArray
{
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray(VertexArray&&) = delete;
        auto operator=(const VertexArray&) -> VertexArray& = delete;
        auto operator=(VertexArray&&) -> VertexArray& = delete;

        auto AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) -> void;

        auto Bind() const -> void;
        auto Unbind() const -> void;
    private:
        uint m_id{};
}; // class VertexArray

} // namespace Renderer::GPU
