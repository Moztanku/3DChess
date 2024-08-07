/**
 * @file VertexArray.cpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Implementation of VertexArray class
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Renderer/GPU/VertexArray.hpp"

namespace Renderer::GPU
{
    
VertexArray::VertexArray() 
{
    glGenVertexArrays(1, &m_id);
}
    
VertexArray::~VertexArray() 
{
    glDeleteVertexArrays(1, &m_id);
}
    
auto VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) -> void 
{
    Bind();
    vb.Bind();
    
    const auto& elements = layout.GetElements();
    uint offset = 0;
    
    for (uint i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<const void*>(offset)); // NOLINT (cppcoreguidelines-pro-type-reinterpret-cast)
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
    
    vb.Unbind();
    Unbind();
}
    
auto VertexArray::Bind() const -> void
{
    glBindVertexArray(m_id);
}
    
auto VertexArray::Unbind() const -> void
{
    glBindVertexArray(0);
}
    
} // namespace Renderer::GPU
