#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "type.hpp"
#include "logger.hpp"
#include "shader.hpp"

#include <iterator>
#include <type_traits>

#include <cassert>
#include <cstdlib>

void frameBufferSizeCallback(GLFWwindow *window, int w, int h);
void processInput(GLFWwindow *window, int key, int scancode, int action, int mods);

constexpr u32 SCR_WIDTH = 800;
constexpr u32 SCR_HEIGHT = 600;

/*
    2

    0       1
*/
constexpr float coords_values[6] = {
    -1.0f, -1.0f, // 0
    1.0f,  -1.0f, // 1
    -1.0f, 1.0f,  // 2
};

constexpr u8 colors_values[9] = {
    255, 0,   0,  // 0
    0,   255, 0,  // 1
    0,   0,   255 // 2
};

int main()
{
    GameProgramming::Logger::init();
    glfwSetErrorCallback(
        [](int code, const char *desc)
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

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH * main_scale, SCR_HEIGHT * main_scale, "Week 1",
                                          nullptr, nullptr);
    if (window == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        LOG_ERROR("Failed to load GL functions");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    LOG_DEBUG("GL_VERSION: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));

    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        style.FontScaleDpi = main_scale;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    glfwSetFramebufferSizeCallback(window, ::frameBufferSizeCallback);
    glfwSetKeyCallback(window, ::processInput);

    enum VBO
    {
        coords,
        colors,
        numVBO
    };
    GLuint vbos[numVBO]{};
    GLuint VAO;
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(numVBO, vbos);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[coords]);
        GLsizeiptr dataSize =
            sizeof(std::remove_all_extents_t<decltype(coords_values)>) * std::size(coords_values);
        glBufferData(GL_ARRAY_BUFFER, // target
                     dataSize,        // size
                     coords_values,   // data
                     GL_STATIC_DRAW   // usage
        );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const GLvoid *)0);

        glBindBuffer(GL_ARRAY_BUFFER, vbos[colors]);
        dataSize =
            sizeof(std::remove_all_extents_t<decltype(colors_values)>) * std::size(colors_values);
        glBufferData(GL_ARRAY_BUFFER, // target
                     dataSize,        // size
                     colors_values,   // data
                     GL_STATIC_DRAW   // usage
        );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 3 * sizeof(u8), (const GLvoid *)0);

        glBindVertexArray(0);
    }

    GameProgramming::Shader::ShaderProgram shaderProgram{};

    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    float view_vec[3] = {0.0f, 0.0f, -4.0f};
    float proj_fov = 30.0f;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
    }
    glBindVertexArray(VAO);
    shaderProgram.use();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("ImGui window");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 20.0f);
        {
            ImGui::SliderFloat3("View", view_vec, -10.0f, 10.0f, "%.2f");
            glm::mat4 view = glm::translate(glm::identity<glm::mat4>(), glm::make_vec3(view_vec));
            shaderProgram.setUniformMatrix4f("view", glm::value_ptr(view));

            ImGui::SliderFloat("Projection FOV", &proj_fov, 20.0f, 90.0f, "%.f");
            glm::mat4 projection = glm::perspective(glm::radians(proj_fov), 4.0f / 3, 0.1f, 100.0f);
            shaderProgram.setUniformMatrix4f("projection", glm::value_ptr(projection));
        }
        ImGui::PopItemWidth();
        ImGui::End();

        glm::mat4 model = glm::rotate(glm::identity<glm::mat4>(), static_cast<float>(glfwGetTime()),
                                      glm::vec3(0.0f, 0.0f, 1.0f));
        shaderProgram.setUniformMatrix4f("model", glm::value_ptr(model));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void frameBufferSizeCallback(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}