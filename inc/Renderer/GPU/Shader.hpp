/**
 * @file Shader.hpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Abstraction of OpenGL Shader programs (vertex and fragment shaders)
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <glm/matrix.hpp>

#include <unordered_map>
#include <filesystem>
#include <string>

#include "jac/type_defs.hpp"

namespace Renderer::GPU
{

class Shader
{
    public:
        Shader(const std::filesystem::path& combined_shader_path);
        Shader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader(Shader&&) = delete;
        auto operator=(const Shader&) -> Shader& = delete;
        auto operator=(Shader&&) -> Shader& = delete;

        auto Bind() const -> void;
        auto Unbind() const -> void;

        template<typename... Args>
        auto SetUniform(const std::string& name, Args... args) -> void;

        template<typename Matrix>
        auto SetUniformM(const std::string& name, const Matrix& matrix) -> void;
    private:
        uint m_id{};
        std::filesystem::path m_FilePath{};
        std::unordered_map<std::string, int> m_UniformLocationCache{};

        auto GetUniformLocation(const std::string& name) -> int;
}; // class Shader

} // namespace Renderer::GPU
