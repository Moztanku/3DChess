/**
 * @file VertexBufferLayout.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Class that describes single vertice (element) in VertexBuffer
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <glad/gl.h>

#include <vector>
#include <cassert>

#include "jac/type_defs.hpp"

namespace Renderer::GPU
{

struct VertexBufferElement 
{
    uint count;
    uint type;
    uchar normalized;

    static auto GetSizeOfType(uint type) -> uint
    {
        switch (type)
        {
            case GL_FLOAT: return sizeof(GLfloat);
            case GL_UNSIGNED_INT: return sizeof(GLuint);
            case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
            default: return 0;
        }
    }
}; // struct VertexBufferElement

class VertexBufferLayout
{
    public:
        template<typename T>
        auto Push(uint /*count*/) -> void
        {
            assert(false);
        }

        [[nodiscard]] inline auto GetElements() const -> const std::vector<VertexBufferElement>& { return m_Elements; }
        [[nodiscard]] inline auto GetStride() const -> uint { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements{};
        uint m_Stride{};
}; // class VertexBufferLayout

} // namespace Renderer::GPU
