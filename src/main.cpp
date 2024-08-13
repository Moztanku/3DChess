#include <iostream>
#include <memory>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>

#include "Renderer/GPU/Shader.hpp"
#include "Renderer/GPU/Texture.hpp"
#include "Renderer/GPU/VertexArray.hpp"
#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/IndexBuffer.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"

#include "Renderer/Chessboard.hpp"
#include "Renderer/Background.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"

#include "Chess/Board.hpp"

auto init_glfw() -> void
{
    if (!glfwInit())
    {
        int ec = glfwGetError(nullptr);

        std::cerr << "Failed to initialize GLFW: " << ec << std::endl;

        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        4  
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        6
    );

    #ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );
    #endif

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    glfwWindowHint(GLFW_SAMPLES, 4);
}

auto create_window() -> GLFWwindow*
{
    init_glfw();

    auto window = glfwCreateWindow(800, 600, "Hello, World!", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    return window;
}

auto init_glad() -> void
{
    if (!gladLoaderLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

auto get_errors() -> void
{
    GLenum error;

    while ((error = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error: " << std::hex << error << std::dec << std::endl;
    }

    return;
}

int w = 8;
int h = 8;
std::unique_ptr<Renderer::Chessboard> chessboard;

auto find_intersection_with_board(
    const glm::vec3& camera_pos,
    const glm::vec3& camera_forward,
    const glm::vec3& camera_up,
    const glm::vec3& camera_right,
    const float fov_y,
    const glm::vec2& mouse_pos) -> glm::vec3
{
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    float fov_x = 2.0f * atan(tan(fov_y / 2.0f) * aspect_ratio);

    const float mouse_x = (mouse_pos.x / width - 0.5f) * 2.0f;
    const float mouse_y = -(mouse_pos.y / height - 0.5f) * 2.0f;

    glm::vec3 ray = glm::normalize(
        camera_forward +
        (mouse_x * tanf(fov_x / 2.0f) * camera_right) +
        (mouse_y * tanf(fov_y / 2.0f) * camera_up)
    );

    float t = -camera_pos.z / ray.z;

    glm::vec3 intersection = camera_pos + t * ray;

    return intersection;
}

auto draw_at(const glm::vec3& pos, const glm::mat4& projView) -> void
{
    static Renderer::GPU::Shader piece_shader{
        "res/shaders/piece.vert",
        "res/shaders/piece.frag"
    };
    static const Renderer::Mesh mesh{
        "res/models/pawn/model.obj"
    };

    glm::mat4 model = glm::mat4(1.0f);

    constexpr float scale = 12.f;

    model = glm::scale(model, glm::vec3(scale, scale, scale));
    model = glm::translate(model, pos / scale);

    piece_shader.Bind();

    piece_shader.SetUniform("uColor", 1.0f, 0.2f, 0.2f);
    piece_shader.SetUniformM("uModel", model);
    piece_shader.SetUniformM("uMVP", projView * model);

    mesh.Draw();

    piece_shader.Unbind();
}

auto main() -> int
{
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{create_window(), glfwDestroyWindow};
    init_glad();

    [[maybe_unused]]
    glm::mat4 model = glm::mat4(1.0f);

    float posX = 300.0f;
    float posY = 0.0f;
        
    Renderer::Camera camera{glm::vec3{posX, posY, 300.f}, glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.f, 1.f, 0.f}};

    glfwSetFramebufferSizeCallback(window.get(), [](GLFWwindow* /* window */, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glEnable(GL_DEPTH_TEST);

    Renderer::Mesh pawn_mesh{"res/models/horse/model.obj"};
    Renderer::GPU::Shader piece_shader{"res/shaders/piece.vert", "res/shaders/piece.frag"};

    glm::mat4 model_pawn = glm::mat4(1.0f);
    constexpr float scale = 12.f;
    model_pawn = glm::scale(model_pawn, glm::vec3(scale, scale, scale));

    Chess::Board board{"res/boards/empty.cfg"};

    while (!glfwWindowShouldClose(window.get()))
    {
        if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS)
            camera.move({0.f, 0.f, 1.f});
        if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS)
            camera.move({0.f, -0.f, -1.f});
        if (glfwGetKey(window.get(), GLFW_KEY_A) == GLFW_PRESS)
            camera.move({-1.f, 0.f, 0.f});
        if (glfwGetKey(window.get(), GLFW_KEY_D) == GLFW_PRESS)
            camera.move({1.f, 0.f, 0.f});

        if (glfwGetKey(window.get(), GLFW_KEY_Q) == GLFW_PRESS)
            camera.yaw(0.1f);
        if (glfwGetKey(window.get(), GLFW_KEY_E) == GLFW_PRESS)
            camera.yaw(-0.1f);

        if (glfwGetKey(window.get(), GLFW_KEY_UP) == GLFW_PRESS)
            camera.pitch(0.1f);
        if (glfwGetKey(window.get(), GLFW_KEY_DOWN) == GLFW_PRESS)
            camera.pitch(-0.1f);

        if (glfwGetKey(window.get(), GLFW_KEY_LEFT) == GLFW_PRESS)
            camera.roll(0.1f);
        if (glfwGetKey(window.get(), GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera.roll(-0.1f);

        //
        double posX, posY;
        glfwGetCursorPos(window.get(), &posX, &posY);
        
        glm::vec3 intersection = find_intersection_with_board(
            camera.getPosition(),
            camera.getForward(),
            camera.getUp(),
            camera.getRight(),
            glm::radians(camera.getFov()),
            {posX, posY}
        );
        std::cout << intersection.x << " " << intersection.y << " " << intersection.z << std::endl;
        Chess::Pos piece_pos = 
        {
            std::floorf((intersection.x - 32.f) / 64.f),
            std::floorf((intersection.y - 32.f) / 64.f)
        };

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Renderer::Background::render("res/textures/background.jpg");

        board.draw(camera.getProjection() * camera.getView());

        if (piece_pos.x >= 0 && piece_pos.x < 8 && piece_pos.y >= 0 && piece_pos.y < 8)
        draw_at(
            glm::vec3{
                piece_pos.x * 64.f + 64.f,
                piece_pos.y * 64.f + 64.f,
                0.f
            },
            camera.getProjection() * camera.getView()
        );

        get_errors();

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    return 0;
};
