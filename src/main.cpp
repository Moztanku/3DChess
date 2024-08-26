#include <iostream>
#include <memory>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Renderer/Camera.hpp"

#include "Renderer/Renderer.hpp"

#include "Chess/Board.hpp"

#include "Controller/Controller.hpp"

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

void* func = nullptr;
void* arg = nullptr;

auto main() -> int
{
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window{create_window(), glfwDestroyWindow};
    init_glad();

    glfwSetFramebufferSizeCallback(window.get(), [](GLFWwindow* /* window */, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    Chess::Board board{"res/boards/standard.cfg"};

    Renderer::Camera camera;

    Controller::Controller controller{
        camera, board, window.get()
    };

    Renderer::Renderer renderer{board, controller};


    while (!glfwWindowShouldClose(window.get()))
    {
        controller.update();
        renderer.render(camera.getProjection() * camera.getView(), window.get());

    }

    return 0;
};
