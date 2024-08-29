#include "Controller/Controller.hpp"

#include <format>
#include <iostream>

#include "Controller/GameState.hpp"
#include "Common.hpp"

namespace
{

constexpr auto find_intersection_with_board(
    const glm::vec3& camera_pos,
    const glm::vec3& camera_forward,
    const glm::vec3& camera_up,
    const glm::vec3& camera_right,
    const float fov_y,
    const glm::vec2& mouse_pos,
    const glm::vec<2, int>& window_size) -> glm::vec3
{
    const int width = window_size.x;
    const int height = window_size.y;
    
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    float fov_x = 2.0f * atan(tan(fov_y / 2.0f) * aspect_ratio);

    const float mouse_x = (mouse_pos.x / width - 0.5f) * 2.0f;
    const float mouse_y = -(mouse_pos.y / height - 0.5f) * 2.0f;

    // Ray we are casting from the camera
    const Ray ray{
        .O = camera_pos,
        .D = glm::normalize(
            camera_forward +
            (mouse_x * tanf(fov_x / 2.0f) * camera_right) +
            (mouse_y * tanf(fov_y / 2.0f) * camera_up))
    };

    // Plane equation is ax + by + cz + d = 0, so in case z = 0, we have:
    const Plane plane = {
        .a = 0.0f,
        .b = 0.0f,
        .c = 1.0f,
        .d = 0.0f
    };

    const glm::vec3 intersection = find_intersection(ray, plane);

    return intersection;
}

template <typename CamSetups>
constexpr auto fill_camera_setup_top(CamSetups& setup, const Chess::Pos board_size) -> void
{
    const float middle_x = board_size.x * square_size / 2.f + border_size;
    const float middle_y = board_size.y * square_size / 2.f + border_size;
    const float height = square_size * std::max(board_size.x, board_size.y) * 0.75f;

    const glm::vec3 pos = {middle_x, middle_y, height};
    const glm::vec3 front = {0.0f, 0.0f, -1.0f};

    setup[0] = {
        .position = pos,
        .front = front,
        .up = {0.0f, 1.0f, 0.0f}
    };

    setup[1] = {
        .position = pos,
        .front = front,
        .up = {-1.0f, 0.0f, 0.0f}
    };

    setup[2] = {
        .position = pos,
        .front = front,
        .up = {0.0f, -1.0f, 0.0f}
    };

    setup[3] = {
        .position = pos,
        .front = front,
        .up = {1.0f, 0.0f, 0.0f}
    };
}

template <typename CamSetups>
constexpr auto fill_camera_setup_sides(CamSetups& setup, const Chess::Pos board_size) -> void
{
    const float width_x = board_size.x * square_size + 2 * border_size;
    const float width_y = board_size.y * square_size + 2 * border_size;
    const float height = square_size * std::max(board_size.x, board_size.y) * 0.5f;
    const float padding = square_size + std::max(board_size.x, board_size.y) * square_size * 0.2f;

    setup[0] = {
        .position = {width_x / 2.0f, -padding, height},
        .front = {0.0f, 1.0f, 0.0f},
        .up = {0.0f, 0.0f, 1.0f}
    };

    setup[1] = {
        .position = {width_x + padding, width_y / 2.0f, height},
        .front = {-1.0f, 0.0f, 0.0f},
        .up = {0.0f, 0.0f, 1.0f}
    };

    setup[2] = {
        .position = {width_x / 2.0f, width_y + padding, height},
        .front = {0.0f, -1.0f, 0.0f},
        .up = {0.0f, 0.0f, 1.0f}
    };

    setup[3] = {
        .position = {-padding, width_y / 2.0f, height},
        .front = {1.0f, 0.0f, 0.0f},
        .up = {0.0f, 0.0f, 1.0f}
    };
}

} // namespace

namespace Controller
{

Controller::Controller(Renderer::Camera& camera, Chess::Board& board, GLFWwindow* window) noexcept:
    m_Camera{camera},
    m_Board{board},
    m_Window{window}
{
    m_Keys[GLFW_KEY_Q] = GLFW_RELEASE;
    m_KeyActions[GLFW_KEY_Q] = Action::PreviousCamera;

    m_Keys[GLFW_KEY_E] = GLFW_RELEASE;
    m_KeyActions[GLFW_KEY_E] = Action::NextCamera;
    
    m_Keys[GLFW_KEY_R] = GLFW_RELEASE;
    m_KeyActions[GLFW_KEY_R] = Action::UndoMove;

    m_Keys[GLFW_KEY_T] = GLFW_RELEASE;
    m_KeyActions[GLFW_KEY_T] = Action::ToggleTopView;

    m_Keys[GLFW_KEY_A] = GLFW_RELEASE;
    m_KeyActions[GLFW_KEY_A] = Action::ToggleAutoRotate;

    m_Keys[GLFW_KEY_M] = GLFW_RELEASE;
    m_KeyActions[GLFW_KEY_M] = Action::ResetBoard;

    fill_camera_setup_top(m_CameraSetupsTop, m_Board.getSize());
    fill_camera_setup_sides(m_CameraSetupsSide, m_Board.getSize());

    update_camera();
}

auto Controller::update() noexcept -> void
{
    glfwPollEvents();
    GameState state = m_Board.getCurrentGameState();

    if (state != GameState::Playing)
    {

    }

    handle_mouse_move();
    
    handle_action(handle_keyboard());
    handle_action(handle_mouse_click());
}

auto Controller::update_camera() noexcept -> void
{
    if (m_CameraAutoRotate)
        m_CameraSetupIndex = m_Board.getCurrentTurn() == Chess::Player::White ? 0 : 2;

    const CameraSetup& setup = m_CameraTopView ?
        m_CameraSetupsTop[m_CameraSetupIndex] :
        m_CameraSetupsSide[m_CameraSetupIndex];

    m_Camera = Renderer::Camera(setup.position, setup.front, setup.up);

    if (!m_CameraTopView)
        m_Camera.pitch(30.f);
}

auto Controller::handle_mouse_move() noexcept -> void
{
    double x, y;
    glfwGetCursorPos(m_Window, &x, &y);

    int w, h;
    glfwGetWindowSize(m_Window, &w, &h);

    glm::vec3 intersection = find_intersection_with_board(
        m_Camera.getPosition(),
        m_Camera.getForward(),
        m_Camera.getUp(),
        m_Camera.getRight(),
        glm::radians(m_Camera.getFov()),
        {x, y},
        {w, h}
    );

    const Chess::Pos pos{
        std::floorf((intersection.x - border_size) / square_size),
        std::floorf((intersection.y - border_size) / square_size)  
    };

    if (Chess::in_bounds(pos, m_Board.getSize()))
        m_FocusedSquare = pos;
    else
        m_FocusedSquare = std::nullopt;
}

auto Controller::handle_keyboard() noexcept -> Action
{
    for (auto& [key, state] : m_Keys)
    {
        const auto new_state = glfwGetKey(m_Window, key);

        if (state == new_state)
            continue;

        state = new_state;

        if (state == GLFW_RELEASE)
            continue;

        return m_KeyActions[key];
    }

    return Action::None;
}

auto Controller::handle_mouse_click() noexcept -> Action
{
    static KeyState last_state = GLFW_RELEASE;
    KeyState state = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);

    if (last_state == state)
        return Action::None;

    last_state = state;

    if (state == GLFW_RELEASE || !m_FocusedSquare.has_value())
        return Action::None;

    if (!m_SelectedSquare.has_value())
        return Action::SelectPiece;

    if (m_SelectedSquare == m_FocusedSquare)
        return Action::UnselectPiece;

    return Action::MakeMove;
}

auto Controller::handle_action(Action action) noexcept -> void
{
    switch (action)
    {
        case Action::PreviousCamera: {
            m_CameraSetupIndex = (m_CameraSetupIndex + 3) % 4;
            break;
        }

        case Action::NextCamera: {
            m_CameraSetupIndex = (m_CameraSetupIndex + 1) % 4;
            break;
        }

        case Action::ToggleTopView: {
            m_CameraTopView = !m_CameraTopView;
            break;
        }

        case Action::ToggleAutoRotate: {
            m_CameraAutoRotate = !m_CameraAutoRotate;
            break;
        }

        case Action::MakeMove: {
            Chess::Move* move = nullptr;

            for (auto& m : m_PossibleMoves)
                if (m.to == m_FocusedSquare.value())
                {
                    move = &m;
                    break;
                }

            if (move)
            {
                m_Board.executeMove(*move);

                m_SelectedSquare = std::nullopt;
                m_PossibleMoves.clear();

                const Chess::Player current_player = m_Board.getCurrentTurn();
            
                m_AttackingPieces.clear();
                m_Board.getPiecesAttackingPos(
                    m_Board.getKingPos(current_player),
                    !current_player,
                    m_AttackingPieces
                );
            }
        }

        case Action::SelectPiece: {

            const auto& pieces = m_Board.getPieces();

            const auto piece = pieces.find(m_FocusedSquare.value());

            if (piece == pieces.end() || piece->second.color != m_Board.getCurrentTurn())
                break;

            m_SelectedSquare = m_FocusedSquare;
            m_PossibleMoves = m_Board.getPossibleMoves(m_SelectedSquare.value());

            break;
        }

        case Action::UnselectPiece: {

            m_SelectedSquare = std::nullopt;
            m_PossibleMoves.clear();

            break;
        }

        case Action::UndoMove: {

            m_SelectedSquare = std::nullopt;
            m_PossibleMoves.clear();
            m_AttackingPieces.clear();

            m_Board.undoMove();

            Chess::Player current_player = m_Board.getCurrentTurn();
            m_Board.getPiecesAttackingPos(
                m_Board.getKingPos(current_player),
                !current_player,
                m_AttackingPieces
            );

            break;
        }

        case Action::ResetBoard: {
            m_Board.reset();

            m_SelectedSquare = std::nullopt;
            m_PossibleMoves.clear();
            m_AttackingPieces.clear();

            break;
        }

        default:
            break;
    }

    update_camera();
}

} // namespace Controller
