#pragma once

#include <string_view>

#include <glm/glm.hpp>

#include "Renderer/GPU/Texture.hpp"

namespace Renderer
{

class UIBox
{
    public:
        struct Settings
        {
            glm::vec2 position{0.f, 0.f};
            glm::vec2 size{1.f, 1.f};
            glm::float32 alpha{1.f};
            glm::vec3 color{1.f, 1.f, 1.f};
        };

        UIBox(const std::string_view tex_path);

        auto draw() const -> void;
        auto draw(const Settings& settings) const -> void;
    private:
        const GPU::Texture m_Texture;
}; // class UIBox

} // namespace Renderer
