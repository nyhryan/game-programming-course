#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "type.hpp"

#include <cstdio>
#include <cstdlib>

constexpr u32 SCR_WIDTH = 800;
constexpr u32 SCR_HEIGHT = 600;

void frameBufferSizeCallback(GLFWwindow *window, int w, int h);
void processInput(GLFWwindow *window);

inline static const char *getString(GLenum name)
{
    return reinterpret_cast<const char*>(glGetString(name));
}

int main()
{
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("[%H:%M:%S] [%^%L%$] %v");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        spdlog::error("Failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, ::frameBufferSizeCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        spdlog::error("Failed to initialize GLAD");
        exit(EXIT_FAILURE);
    }

    spdlog::debug("GL_VERSION: {}", ::getString(GL_VERSION));

    while (!glfwWindowShouldClose(window))
    {
        ::processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    spdlog::shutdown();
    glfwTerminate();
}

void frameBufferSizeCallback(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
    spdlog::debug("[Frame buffer size callback] w: {}, h: {}", w, h);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}