/**
 * @file IndexBuffer.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Index Buffer (EBO), holds indices of vertex data
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

class IndexBuffer
{
    public:
        IndexBuffer(const uint* data, const uint count);
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&&) = delete;
        auto operator=(const IndexBuffer&) -> IndexBuffer& = delete;
        auto operator=(IndexBuffer&&) -> IndexBuffer& = delete;

        auto Bind() const -> void;
        auto Unbind() const -> void;

        [[nodiscard]] inline auto GetCount() const -> uint { return m_Count; }
    private:
        uint m_id{};
        uint m_Count{};
}; // class IndexBuffer

} // namespace Renderer::GPU
