#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "type.hpp"
#include "logger.hpp"

#include <cstdio>
#include <cstdlib>

constexpr u32 SCR_WIDTH = 800;
constexpr u32 SCR_HEIGHT = 600;

void frameBufferSizeCallback(GLFWwindow *window, int w, int h);
void processInput(GLFWwindow *window, int key, int scancode, int action, int mods);

int main()
{
    GameProgramming::Logger::init();
    glfwSetErrorCallback([](int code, const char *desc)
    {
        LOG_ERROR("[GLFW] Code: {:#010x}, Desc: {}", code, desc);
    });

    if (!glfwInit())
    {
        LOG_ERROR("Failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, ::frameBufferSizeCallback);
    glfwSetKeyCallback(window, ::processInput);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        LOG_ERROR("Failed to load GL functions");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    LOG_DEBUG("GL_VERSION: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

void frameBufferSizeCallback(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
    LOG_DEBUG("[Frame buffer size callback] w: {}, h: {}", w, h);
}

void processInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}