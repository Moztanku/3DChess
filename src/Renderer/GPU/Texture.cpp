/**
 * @file Texture.cpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Implementation of Texture class
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Renderer/GPU/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "jac/require.hpp"
#include "jac/debug.hpp"

namespace Renderer::GPU
{
    
Texture::Texture(const std::filesystem::path& path)
{
    stbi_set_flip_vertically_on_load(true);
    uchar* data = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannels, 0); // NOLINT (clang-analyzer-unix.Malloc)
    
    if (data == nullptr)
    {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }

    if (m_nrChannels != 3 && m_nrChannels != 4)
    {
        std::cerr << "Number of color channels should be either 3 or 4" << std::endl;
        stbi_image_free(data);
        return;
    }
    
    glGenTextures(1, &m_id);
    
    if (m_id == 0)
    {
        std::cerr << "Failed to generate texture" << std::endl;
        stbi_image_free(data);
        return;
    }
    
    glBindTexture(GL_TEXTURE_2D, m_id);
    
    int format = m_nrChannels == 4? GL_RGBA: GL_RGB;
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
}
    
Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

auto Texture::Bind() const -> void
{
    glActiveTexture(GL_TEXTURE0 + m_slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
    
auto Texture::Bind(uint slot) -> void
{
    if constexpr(Debug)
        JAC_REQUIRE(slot < 32);
    
    m_slot = slot;
    Bind();
}
    
auto Texture::Unbind() const -> void
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
    
} // namespace Renderer::GPU
