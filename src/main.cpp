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
        std::cerr << "OpenGL Error: " << error << std::endl;
    }

    return;
}

int w = 8;
int h = 8;
std::unique_ptr<Renderer::Chessboard> chessboard;

auto main() -> int
{
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{create_window(), glfwDestroyWindow};
    init_glad();

    [[maybe_unused]]
    glm::mat4 model = glm::mat4(1.0f);

    float posX = 300.0f;
    float posY = 0.0f;
        
    Renderer::Camera camera{glm::vec3{posX, posY, 300.f}, glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.f, 1.f, 0.f}};

    chessboard = std::make_unique<Renderer::Chessboard>(w, h);

    glfwSetFramebufferSizeCallback(window.get(), [](GLFWwindow* /* window */, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glEnable(GL_DEPTH_TEST);

    Renderer::Mesh pawn_mesh{"res/models/pawn/model.obj"};
    Renderer::GPU::Shader piece_shader{"res/shaders/piece.vert", "res/shaders/piece.frag"};

    glm::mat4 model_pawn = glm::mat4(1.0f);
    constexpr float scale = 16.f;
    model_pawn = glm::scale(model_pawn, glm::vec3(scale, scale, scale));

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Renderer::Background::render("res/textures/background.jpg");

        chessboard->render(camera.getProjection() * camera.getView() * model);

        piece_shader.Bind();

        piece_shader.SetUniform("uColor", 0.2f, 0.2f, 0.2f);
        piece_shader.SetUniform("uLightDir", 0.0f, 0.0f, 1.0f);
        piece_shader.SetUniform("uLightColor", 1.0f, 1.0f, 1.0f);

        for (int i = 0; i < w; i++)
        {
            constexpr float border = 32.f / (scale);
            constexpr float square = 64.f / (scale);

            const float x = i * square + 2 * border;
            const float y = 1 * square + 2 * border;

            [[maybe_unused]]
            const glm::mat4 new_model = glm::translate(model_pawn, glm::vec3(x, y, 0.f));
            piece_shader.SetUniformM("uModel", new_model);
            piece_shader.SetUniformM("uMVP", camera.getProjection() * camera.getView() * new_model);

            pawn_mesh.Draw();
        }

        piece_shader.Unbind();

        get_errors();

        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }

    return 0;
};
