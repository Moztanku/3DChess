#pragma once

#include <memory>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Chess/Board.hpp"
#include "Controller/Controller.hpp"
#include "Renderer/Chessboard.hpp"

namespace Renderer
{

class Renderer
{
    public:
        Renderer(const Chess::Board& board, const Controller::Controller& controller);

        auto render(const glm::mat4& projView, GLFWwindow* window) const -> void;
    private:
        const Chess::Board& m_board;
        const Controller::Controller& m_controller;

        // turn into a Mesh class
        const std::unique_ptr<Chessboard> m_board_mesh;
};

} // namespace Renderer