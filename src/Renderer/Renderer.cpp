#include "Renderer/Renderer.hpp"

#include <map>
#include <optional>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/UIBox.hpp"
#include "Renderer/GPU/Shader.hpp"
#include "Chess/Piece.hpp"
#include "Common.hpp"

namespace
{

inline auto load_meshes(const std::map<Chess::Piece::Type, std::string>& paths) ->
    std::map<Chess::Piece::Type, Renderer::Mesh>
{
    std::map<Chess::Piece::Type, Renderer::Mesh> meshes;

    for (const auto& [type, path] : paths)
        meshes.emplace(type, Renderer::Mesh{path});

    return meshes;
}

inline auto get_mesh(const Chess::Piece::Type type) -> const Renderer::Mesh&
{
    using Renderer::Mesh;
    using Type = Chess::Piece::Type;

    const std::map<Type, std::string> paths = {
        {Type::Pawn, "res/models/pawn/model.obj"},
        {Type::Knight, "res/models/horse/model.obj"},
        {Type::Bishop, "res/models/bishop/model.obj"},
        {Type::Rook, "res/models/rook/model.obj"},
        {Type::Queen, "res/models/queen/model.obj"},
        {Type::King, "res/models/king/model.obj"}
    };

    static const std::map<Type, Mesh> meshes = load_meshes(paths);

    return meshes.at(type);
}

auto draw_piece(
    const glm::mat4& projView,
    const Chess::Pos& pos,
    const Chess::Piece& piece) -> void
{
    using namespace Chess;

    static Renderer::GPU::Shader piece_shader{
        "res/shaders/piece.vert",
        "res/shaders/piece.frag"
    };

    glm::mat4 model = glm::mat4(1.0f);

    constexpr float scale = 12.f;
    constexpr float square = square_size / scale;
    constexpr float border = border_size / scale;

    model = glm::scale(model, glm::vec3(scale, scale, scale));

    const float x = pos.x * square + 2 * border;
    const float y = pos.y * square + 2 * border;

    model = glm::translate(model, glm::vec3(x, y, 0.f));

    if (piece.color == Player::White)
        model = glm::rotate(
            model, glm::radians(180.f),
            glm::vec3(0.f, 0.f, 1.f)
        );

    piece_shader.Bind();

    const float mesh_color =
        piece.color == Player::White ? 0.9f : 0.2f;

    piece_shader.SetUniform("uColor", mesh_color, mesh_color, mesh_color);
    piece_shader.SetUniformM("uModel", model);
    piece_shader.SetUniformM("uMVP", projView * model);

    get_mesh(piece.type).draw();

    piece_shader.Unbind();
}

auto init_vbo() -> Renderer::GPU::VertexBuffer
{
    using namespace Renderer::GPU;

    constexpr std::array<float, 12> vertices = {
        0.f, 0.f,
        1.f, 0.f,
        1.f, 1.f,

        1.f, 1.f,
        0.f, 1.f,
        0.f, 0.f
    };

    return VertexBuffer{vertices.data(), vertices.size() * sizeof(float)};
}

auto init_vao(const Renderer::GPU::VertexBuffer& vbo) -> Renderer::GPU::VertexArray
{
    using namespace Renderer::GPU;

    VertexBufferLayout layout;
    layout.Push<float>(2);

    VertexArray vao;
    vao.AddBuffer(vbo, layout);

    return vao;
}

auto draw_rect(
    const glm::mat4& projView,
    const Chess::Pos& pos,
    const glm::vec4& color) -> void
{
    using namespace Renderer::GPU;

    static Shader shader {
        "res/shaders/rect.vert",
        "res/shaders/rect.frag"
    };

    static const VertexBuffer vbo = init_vbo();
    static const VertexArray vao = init_vao(vbo);

    const glm::vec3 coords{
        pos.x + 0.5f,
        pos.y + 0.5f,
        0.01f // Z set to more than 0 to avoid z-fighting with chessboard
    };

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(square_size, square_size, square_size));
    model = glm::translate(model, coords);

    shader.Bind();

    shader.SetUniform("uRGBA", color.r, color.g, color.b, color.a);
    shader.SetUniformM("uModel", model);
    shader.SetUniformM("uProjView", projView);

    vao.Bind();
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    vao.Unbind();

    shader.Unbind();
}

auto draw_game_over_screen(const Controller::GameState state, const float alpha) -> void
{
    using Renderer::UIBox;
    using Controller::GameState;

    static const UIBox draw{"res/textures/gameover-draw.jpg"};
    static const UIBox white_won{"res/textures/gameover-white-won.jpg"};
    static const UIBox black_won{"res/textures/gameover-black-won.jpg"};

    const UIBox* box =
        state == GameState::Draw ? &draw :
        state == GameState::WhiteWin ? &white_won :
        &black_won;

    box->draw({
        .position = {.1f, .1f},
        .size = {0.8f, 0.8f},
        .alpha = alpha
    });
}

} // namespace

namespace Renderer
{

Renderer::Renderer(const Chess::Board& board, const Controller::Controller& controller)
    : m_Board{board},
    m_Controller{controller},
    m_BoardMesh{std::make_unique<Chessboard>(board.getSize())}
{
    glEnable(GL_DEPTH_TEST);
}

auto Renderer::render(const glm::mat4& projView, GLFWwindow* window) const -> void
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw background
    static const UIBox background{"res/textures/background.jpg"};
    background.draw();

    // Draw chessboard
    m_BoardMesh->draw(projView);

    // Draw pieces
    for (const auto& [pos, piece] : m_Board.getPieces())
        draw_piece(projView, pos, piece);

    // Draw focused square
    const std::optional<Chess::Pos>& focused = m_Controller.getFocusedPiece();

    if (focused)
        draw_rect(projView, *focused, {0.1f, 0.9f, 0.1f, 1.0f});

    // Draw selected square
    const std::optional<Chess::Pos>& selected = m_Controller.getSelectedPiece();

    if (selected && (!focused || *selected != *focused))
        draw_rect(projView, *selected, {0.9f, 1.0f, 0.0f, 0.8f});

    // Draw possible moves
    for (const auto& move : m_Controller.getPossibleMoves())
        if (!focused || move.to != *focused)
            draw_rect(projView, move.to, {0.0f, 0.8f, 0.1f, 0.8f});

    // Draw pieces attacking king
    for (const auto& pos : m_Controller.getAttackingPieces())
        draw_rect(projView, pos, {0.8f, 0.0f, 0.1f, 0.8f});

    // Draw game over screen
    static double game_over_start = 0.0;
    static double game_over_timer = 0.0;

    using Controller::GameState;
    const GameState state = m_Board.getCurrentGameState();

    if (state != GameState::Playing)
    {
        if (game_over_start == 0.0)
            game_over_start = glfwGetTime();

        game_over_timer = glfwGetTime() - game_over_start;

        const float fade_time_s = 2.5f;
        const float alpha = std::min(0.975f, static_cast<float>(game_over_timer) / fade_time_s);

        draw_game_over_screen(state, alpha);
    } else
    {
        game_over_start = 0.0;
        game_over_timer = 0.0;
    }
    
    glfwSwapBuffers(window);
}

} // namespace Renderer
