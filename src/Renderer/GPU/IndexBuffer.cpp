/**
 * @file IndexBuffer.cpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Implementation of IndexBuffer class
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Renderer/GPU/IndexBuffer.hpp"

#include <glad/gl.h>

namespace Renderer::GPU
{

IndexBuffer::IndexBuffer(const uint* data, const uint count) : m_Count{count} {
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * sizeof(uint), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_id);
}

auto IndexBuffer::Bind() const -> void {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

auto IndexBuffer::Unbind() const -> void {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace Renderer::GPU
