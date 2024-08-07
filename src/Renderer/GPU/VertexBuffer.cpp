/**
 * @file VertexBuffer.cpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Implementation of VertexBuffer class
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Renderer/GPU/VertexBuffer.hpp"

#include <glad/gl.h>

namespace Renderer::GPU
{
    
VertexBuffer::VertexBuffer(const void* data, uint size) : m_Count(size / sizeof(float)) {
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, GL_STATIC_DRAW);
}
    
VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_id);
}
    
auto VertexBuffer::Bind() const -> void {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}
    
auto VertexBuffer::Unbind() const -> void {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
    
} // namespace Renderer::GPU
