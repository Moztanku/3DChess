/**
 * @file Texture.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Texture
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <glad/gl.h>

#include <iostream>
#include <filesystem>

#include "jac/type_defs.hpp"

namespace Renderer::GPU
{

class Texture
{
    public:
        Texture(const std::filesystem::path& path);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        auto operator=(const Texture&) -> Texture& = delete;
        auto operator=(Texture&&) -> Texture& = delete;

        auto Bind() const -> void;
        auto Bind(uint slot) -> void;
        auto Unbind() const -> void;

        [[nodiscard]] auto GetTextureParameters() const {
            struct {
                int widht;
                int height;
                int nrChannels;
            } params;

            params.widht = m_width;
            params.height = m_height;
            params.nrChannels = m_nrChannels;

            return params;
        }
    private:
        uint m_id{};
        uint m_slot{};

        int m_width{}, m_height{}, m_nrChannels{};

        // TODO: Different texture wrap modes and filters
        // GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER
}; // class Texture

} // namespace Renderer::GPU
