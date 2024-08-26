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

template <typename CamSetups>
auto set_camera(
    Renderer::Camera& camera,
    const CamSetups& side,
    const CamSetups& top,
    const size_t index,
    const bool top_view) -> void
{
    const auto& setup = top_view ? top[index] : side[index];
    camera = Renderer::Camera(setup.position, setup.front, setup.up);

    if (!top_view)
        camera.pitch(30.f);
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
    m_Keys[GLFW_KEY_E] = GLFW_RELEASE;
    m_Keys[GLFW_KEY_R] = GLFW_RELEASE;
    m_Keys[GLFW_KEY_T] = GLFW_RELEASE;
    m_Keys[GLFW_KEY_A] = GLFW_RELEASE;
    m_Keys[GLFW_KEY_M] = GLFW_RELEASE;

    fill_camera_setup_top(m_CameraSetupsTop, m_Board.get_size());
    fill_camera_setup_sides(m_CameraSetupsSide, m_Board.get_size());

    set_camera(
        m_Camera,
        m_CameraSetupsSide,
        m_CameraSetupsTop,
        m_CameraSetupIndex,
        m_CameraTopView);
}

auto Controller::update() noexcept -> void
{
    glfwPollEvents();
    GameState state = m_Board.get_current_game_state();

    if (state != GameState::Playing)
    {

    }
    
    handle_keyboard();
    handle_mouse_move();
    handle_mouse_click();
}

auto Controller::handle_keyboard() noexcept -> void
{
    for (auto& [key, state] : m_Keys)
    {
        const auto new_state = glfwGetKey(m_Window, key);

        if (state == new_state)
            continue;

        state = new_state;

        if (state == GLFW_RELEASE)
            continue;

        switch (key)
        {
            case GLFW_KEY_Q:
                m_CameraSetupIndex = (m_CameraSetupIndex - 2) % 4;
            case GLFW_KEY_E:
                m_CameraSetupIndex = (m_CameraSetupIndex + 1) % 4;
                m_CameraTopView = !m_CameraTopView;
            case GLFW_KEY_T:
                m_CameraTopView = !m_CameraTopView;
                
                set_camera(
                    m_Camera,
                    m_CameraSetupsSide,
                    m_CameraSetupsTop,
                    m_CameraSetupIndex,
                    m_CameraTopView);
                break;

            case GLFW_KEY_R:
                m_Board.undo_move();
                break;
            
            case GLFW_KEY_A:
                m_CameraAutoRotate = !m_CameraAutoRotate;
                break;

            case GLFW_KEY_M:
                m_Board.reset();
                break;
        }
    }
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

    if (Chess::in_bounds(pos, m_Board.get_size()))
        m_FocusedSquare = pos;
    else
        m_FocusedSquare = std::nullopt;
}

auto Controller::handle_mouse_click() noexcept -> void
{
    static KeyState last_state = GLFW_RELEASE;
    KeyState state = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);

    if (last_state == state)
        return;

    last_state = state;

    if (state == GLFW_RELEASE || !m_FocusedSquare.has_value())
        return;

    if (!m_SelectedSquare.has_value())
    {
        m_SelectedSquare = m_FocusedSquare;
        m_PossibleMoves = m_Board.get_possible_moves(m_FocusedSquare.value());
    } else if (m_SelectedSquare == m_FocusedSquare)
    {
        m_SelectedSquare = std::nullopt;
        m_PossibleMoves.clear();
    } else
    {
        auto move = std::find_if(
            m_PossibleMoves.begin(),
            m_PossibleMoves.end(),
            [this](const Chess::Move& move) {
                return move.to == m_FocusedSquare.value();
            }
        );

        if (move != m_PossibleMoves.end())
        {
            m_Board.execute_move(*move);

            m_SelectedSquare = std::nullopt;
            m_PossibleMoves.clear();

            if (m_CameraAutoRotate)
            {
                m_CameraSetupIndex = m_Board.get_current_turn() == Chess::Player::White ? 0 : 2;
                set_camera(
                    m_Camera,
                    m_CameraSetupsSide,
                    m_CameraSetupsTop,
                    m_CameraSetupIndex,
                    m_CameraTopView);
            }
        } else
        {
            m_SelectedSquare = m_FocusedSquare;
            m_PossibleMoves = m_Board.get_possible_moves(m_FocusedSquare.value());
        }
    }
    
    if (m_SelectedSquare.has_value() && m_PossibleMoves.empty())
        m_SelectedSquare = std::nullopt;
}

} // namespace Controller
