/**
 * @file VertexBuffer.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Vertex Buffer Object (VBO), holds vertex data
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "jac/type_defs.hpp"

namespace Renderer::GPU
{

class VertexBuffer
{
    public:
        VertexBuffer(const void* data,  uint size);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;
        auto operator=(const VertexBuffer&) -> VertexBuffer& = delete;
        auto operator=(VertexBuffer&&) -> VertexBuffer& = delete;

        auto Bind() const -> void;
        auto Unbind() const -> void;

        [[nodiscard]] inline auto GetCount() const -> uint { return m_Count; }
    private:
        uint m_id{};
        uint m_Count{};
}; // class VertexBuffer

} // namespace Renderer::GPU
