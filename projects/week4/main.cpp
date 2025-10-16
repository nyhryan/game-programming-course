#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "shader.hpp"
#include "logger.hpp"
#include "type.hpp"
#include "camera.h"

#include "teapot_loader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

constexpr u32 SCR_WIDTH = 800;
constexpr u32 SCR_HEIGHT = 800;

bool isF1KeyPressed = false;
bool showImGuiOverlay = true;

// camera
Camera camera(glm::vec3(0.0f, 0.5f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float lightPosArray[3] {1.2f, 1.0f, 2.0f};

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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH * main_scale, SCR_HEIGHT * main_scale, "Week 2",
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

    // ImGUI setup
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        style.WindowRounding = 4.0f;
        style.FrameRounding = 2.0f;
        style.FontScaleDpi = main_scale;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    GameProgramming::Shader::ShaderProgram lampShader{RESOURCE_PATH_PREFIX "shaders/14.2.lamp.vs",
                                                      RESOURCE_PATH_PREFIX "shaders/14.2.lamp.fs"};

    GameProgramming::Shader::ShaderProgram teapotShader1{
        RESOURCE_PATH_PREFIX "shaders/14.2.teapot.vs",
        RESOURCE_PATH_PREFIX "shaders/14.2.teapot.fs"};

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

    // first, configure the lamp's VAO and VBO
    unsigned int VBO, lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(lightVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // teapot VAO and VBO
    std::vector<float> data;
    Teapot teapot(RESOURCE_PATH_PREFIX "other/teapot.vbo", data, 8);

    unsigned int teapotVBO, teapotVAO;
    glGenVertexArrays(1, &teapotVAO);
    glGenBuffers(1, &teapotVBO);
    glBindBuffer(GL_ARRAY_BUFFER, teapotVBO);
    glBufferData(GL_ARRAY_BUFFER, teapot.nVertNum * teapot.nVertFloats * sizeof(float), &data[0],
                 GL_STATIC_DRAW);

    glBindVertexArray(teapotVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, teapot.nVertFloats * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, teapot.nVertFloats * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, teapot.nVertFloats * sizeof(float),
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        teapotShader1.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        teapotShader1.setUniformMatrix4f("projection", projection);
        teapotShader1.setUniformMatrix4f("view", view);
        // eye position
        teapotShader1.setUniformVec3("eyePos", camera.Position);

        // draw the teapot object 1
        // light properties
        glm::vec3 lightColor(1.0, 1.0, 1.0);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);   // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
        teapotShader1.setUniformVec3("light.ambient", ambientColor);
        teapotShader1.setUniformVec3("light.diffuse", diffuseColor);
        teapotShader1.setUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);
        teapotShader1.setUniformVec3("light.position", lightPos);
        // material properties
        teapotShader1.setUniformVec3("material.ambient", 0.7f, 0.5f, 0.3f);
        teapotShader1.setUniformVec3("material.diffuse", 0.7f, 0.5f, 0.3f);
        teapotShader1.setUniformVec3(
            "material.specular", 0.5f, 0.5f,
            0.5f); // specular lighting doesn't have full effect on this object's material
        teapotShader1.setUniformFloat("material.shininess", 30.0f);
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        teapotShader1.setUniformMatrix4f("model", model);
        // render the teapot
        glBindVertexArray(teapotVAO);
        glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);

        // draw the teapot object 2
        // light properties
        lightColor = glm::vec3(1.0, 1.0, 1.0);
        diffuseColor = lightColor * glm::vec3(0.5f);   // decrease the influence
        ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
        teapotShader1.setUniformVec3("light.ambient", ambientColor);
        teapotShader1.setUniformVec3("light.diffuse", diffuseColor);
        teapotShader1.setUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);
        teapotShader1.setUniformVec3("light.position", lightPos);
        // material properties
        teapotShader1.setUniformVec3("material.ambient", 0.7f, 0.5f, 0.3f);
        teapotShader1.setUniformVec3("material.diffuse", 0.7f, 0.5f, 0.3f);
        teapotShader1.setUniformVec3(
            "material.specular", 0.5f, 0.5f,
            0.5f); // specular lighting doesn't have full effect on this object's material
        teapotShader1.setUniformFloat("material.shininess", 2.0f);
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        teapotShader1.setUniformMatrix4f("model", model);
        // render the teapot
        glBindVertexArray(teapotVAO);
        glDrawArrays(GL_TRIANGLES, 0, teapot.nVertNum);

        // draw the teapot object 3
        // light properties
        lightColor.x = 8.f * sin(glfwGetTime() * 2.0f);
        lightColor.y = 16.f * sin(glfwGetTime() * 0.7f);
        lightColor.z = 32.f * sin(glfwGetTime() * 1.3f);
        diffuseColor = lightColor * glm::vec3(0.5f);   // decrease the influence
        ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
        teapotShader1.setUniformVec3("light.ambient", ambientColor);
        teapotShader1.setUniformVec3("light.diffuse", diffuseColor);
        teapotShader1.setUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);
        teapotShader1.setUniformVec3("light.position", lightPos);
        // material properties
        teapotShader1.setUniformVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        teapotShader1.setUniformVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        teapotShader1.setUniformVec3(
            "material.specular", 0.5f, 0.5f,
            0.5f); // specular lighting doesn't have full effect on this object's material
        teapotShader1.setUniformFloat("material.shininess", 10.0f);
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        teapotShader1.setUniformMatrix4f("model", model);
        // render the teapot
        glBindVertexArray(teapotVAO);
        glDrawArrays(GL_LINES, 0, teapot.nVertNum);

        // also draw the lamp object
        lampShader.use();
        lampShader.setUniformMatrix4f("projection", projection);
        lampShader.setUniformMatrix4f("view", view);
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lampShader.setUniformMatrix4f("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        if (showImGuiOverlay)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (ImGui::Begin("Tea pots!", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Press F1 to toggle this overlay");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                       1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::SliderFloat3("Light Pos", lightPosArray, -1.0f, 1.0f);
                {
                    lightPos = glm::make_vec3(lightPosArray);
                }
            }
            ImGui::End();
            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &teapotVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &teapotVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        if (!isF1KeyPressed)
        {
            showImGuiOverlay = !showImGuiOverlay;

            if (showImGuiOverlay)
            {
                // Show cursor when ImGui overlay is active
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else
            {
                // Hide cursor for camera movement
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                firstMouse = true; // Reset mouse state
            }
            isF1KeyPressed = true;
        }
    }
    else 
    {
        isF1KeyPressed = false;
    }

    ImGuiIO &io = ImGui::GetIO();
    bool imguiWantsInput = showImGuiOverlay && (io.WantCaptureKeyboard || io.WantCaptureMouse);
    if (imguiWantsInput) 
        return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
