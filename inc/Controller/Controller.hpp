#pragma once

#include <map>
#include <functional>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Renderer/Camera.hpp"
#include "Chess/Board.hpp"

namespace Controller
{

class Controller
{
    using Key = int;
    using KeyState = int;

    struct CameraSetup {
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
    };
    public:
        Controller(Renderer::Camera& camera, Chess::Board& board, GLFWwindow* window) noexcept;
        auto update() noexcept -> void;

        auto get_focus() const noexcept -> const std::optional<Chess::Pos>& { return m_FocusedSquare; }
        auto get_selected() const noexcept -> const std::optional<Chess::Pos>& { return m_SelectedSquare; }
        auto get_moves() const noexcept -> const std::vector<Chess::Move>& { return m_PossibleMoves; }
    private:
        Renderer::Camera& m_Camera;
        Chess::Board& m_Board;
        GLFWwindow* m_Window;

        std::map<Key, KeyState> m_Keys;

        std::optional<Chess::Pos> m_FocusedSquare{std::nullopt};
        std::optional<Chess::Pos> m_SelectedSquare{std::nullopt};
        std::vector<Chess::Move> m_PossibleMoves{};

        std::array<CameraSetup, 4> m_CameraSetupsSide;
        std::array<CameraSetup, 4> m_CameraSetupsTop;
        size_t m_CameraSetupIndex{0};
        bool m_CameraTopView{false};
        bool m_CameraAutoRotate{false};

        auto handle_keyboard() noexcept -> void;
        auto handle_mouse_move() noexcept -> void;
        auto handle_mouse_click() noexcept -> void;
}; // class Controller

} // namespace Controller
