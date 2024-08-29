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
    public:
        Controller(Renderer::Camera& camera, Chess::Board& board, GLFWwindow* window) noexcept;
        auto update() noexcept -> void;

        auto getFocusedPiece() const noexcept -> const std::optional<Chess::Pos>& { return m_FocusedSquare; }
        auto getSelectedPiece() const noexcept -> const std::optional<Chess::Pos>& { return m_SelectedSquare; }
        auto getAttackingPieces() const noexcept -> const std::vector<Chess::Pos>& { return m_AttackingPieces; }
        
        auto getPossibleMoves() const noexcept -> const std::vector<Chess::Move>& { return m_PossibleMoves; }
    private:
        Renderer::Camera& m_Camera;
        Chess::Board& m_Board;
        GLFWwindow* m_Window;

        using Key = int;
        using KeyState = int;

        enum class Action
        {
            None,

            PreviousCamera,
            NextCamera,

            ToggleTopView,
            ToggleAutoRotate,

            MakeMove,
            SelectPiece,
            UnselectPiece,

            UndoMove,
            ResetBoard
        };

        std::map<Key, KeyState> m_Keys;
        std::map<Key, Action> m_KeyActions;

        std::optional<Chess::Pos> m_FocusedSquare{std::nullopt};
        std::optional<Chess::Pos> m_SelectedSquare{std::nullopt};
        std::vector<Chess::Pos> m_AttackingPieces{};
        std::vector<Chess::Move> m_PossibleMoves{};

        struct CameraSetup {
            glm::vec3 position;
            glm::vec3 front;
            glm::vec3 up;
        };

        std::array<CameraSetup, 4> m_CameraSetupsSide;
        std::array<CameraSetup, 4> m_CameraSetupsTop;
        size_t m_CameraSetupIndex{0};
        bool m_CameraTopView{false};
        bool m_CameraAutoRotate{false};

        auto update_camera() noexcept -> void;

        auto handle_mouse_move() noexcept -> void;

        auto handle_keyboard() noexcept -> Action;
        auto handle_mouse_click() noexcept -> Action;

        auto handle_action(Action action) noexcept -> void;

}; // class Controller

} // namespace Controller
