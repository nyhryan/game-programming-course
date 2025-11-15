#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "_shader.h"
//#include "logger.hpp"
#include "type.hpp"
#include "camera.h"

#ifndef RESOURCE_PATH_PREFIX
#define RESOURCE_PATH_PREFIX ""
#endif

#pragma region C++ iostream logger
#include <iostream>
#include <format>
#ifndef LOG_ERROR
#define LOG_ERROR(fmt, ...) std::cerr << std::format(fmt __VA_OPT__(, ) __VA_ARGS__) << '\n'
#endif

#ifndef LOG_INFO
#define LOG_INFO(fmt, ...) std::cout << std::format(fmt __VA_OPT__(, ) __VA_ARGS__) << '\n'
#endif

#ifndef LOG_INIT
#define LOG_INIT()
#endif
#pragma endregion

constexpr u32 SCR_WIDTH = 1000, SCR_HEIGHT = 1000;

bool isF1KeyPressed = false;
bool showImGuiOverlay = true;

// camera
Camera camera(glm::vec3(0.0f, 0.5f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
glm::mat4 projection, view, model;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0, 1.0, 1.0);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f);   // decrease the influence
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence

// Ball
GLuint sphereVAO;
int nSphereVert, nSphereAttr;
float drag = 0.99f;
float gravity_strength = 0.81f;
glm::vec3 gravity(0.0f, -gravity_strength, 0.0f);
glm::vec3 ball_initialShootingDirection = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 ball_velocity = gravity_strength * ball_initialShootingDirection;

GLuint ballTexture;
glm::vec3 ball_initialPos(0.0f, 5.0f, 0.0f);
glm::vec3 ball_currentPos = ball_initialPos;
float ball_radius = 1.f;
bool ball_hasStopped = false;
float ball_stopTimerInitialValue = 2.0f;
float ball_stopTimer = 0.0f;

// Floor
GLuint woodTexture, woodFloorVAO, woodFloorEBO;
float woodFloor[] = {
    // positions            // normals         // texcoords
    25.0f, -0.5f, 25.0f, 0.0f,  1.0f,   0.0f,  25.0f,  0.0f, -25.0f, -0.5f, 25.0f,  0.0f,
    1.0f,  0.0f,  0.0f,  0.0f,  -25.0f, -0.5f, -25.0f, 0.0f, 1.0f,   0.0f,  0.0f,   25.0f,

    25.0f, -0.5f, 25.0f, 0.0f,  1.0f,   0.0f,  25.0f,  0.0f, -25.0f, -0.5f, -25.0f, 0.0f,
    1.0f,  0.0f,  0.0f,  25.0f, 25.0f,  -0.5f, -25.0f, 0.0f, 1.0f,   0.0f,  25.0f,  25.0f};

// Debug light cube
float lightCubeVertices[] = {
    // back face
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
    1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
    // front face
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                        // right face
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
    1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
    1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
};
GLuint lightCubeVAO, lightCubeVBO, lightCubeTexture;

// Shadow mapping
GLuint depthMapFBO, depthMapTexture;

GLuint quadVBO, quadVAO;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void loadTexture(GLuint &textureID, const char *path);
void init_sphere(float **, int *, int *);
void renderScene(const Shader &shader);
void renderQuad();

int main()
{
#pragma region Bootstrap
    LOG_INIT();
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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH * main_scale, SCR_HEIGHT * main_scale, "2291012 Yun Hyeok Nam", nullptr, nullptr);
    if (window == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        LOG_ERROR("Failed to load GL functions");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
#pragma endregion

#pragma region ImGUI setup
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
#pragma endregion

#pragma region Ball Setup
    loadTexture(ballTexture, RESOURCE_PATH_PREFIX "textures/2k_earth_daymap.jpg");
    float *sphereVerts = nullptr;
    init_sphere(&sphereVerts, &nSphereVert, &nSphereAttr);
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    {
        GLuint sphereVBO;
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, nSphereVert * nSphereAttr * sizeof(float), sphereVerts, GL_STATIC_DRAW);
        free(sphereVerts);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nSphereAttr * sizeof(float), (const GLvoid *)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nSphereAttr * sizeof(float), (const GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texCoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, nSphereAttr * sizeof(float), (const GLvoid *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
#pragma endregion

#pragma region Floor Setup
    loadTexture(woodTexture, RESOURCE_PATH_PREFIX "textures/wood.jpg");

    glGenVertexArrays(1, &woodFloorVAO);
    glBindVertexArray(woodFloorVAO);
    {
        GLuint woodFloorVBO;
        glGenBuffers(1, &woodFloorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, woodFloorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(woodFloor), woodFloor, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
#pragma endregion

#pragma region Light cube setup (Debug)
    loadTexture(lightCubeTexture, RESOURCE_PATH_PREFIX "textures/bulb.jpg");
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    {
        GLuint lightCubeVBO;
        glGenBuffers(1, &lightCubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const GLvoid *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
#pragma endregion

#pragma region Shadow map setup
    constexpr u32 SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

#pragma endregion

    Shader shader{RESOURCE_PATH_PREFIX "shaders/shadow_mapping.vs", RESOURCE_PATH_PREFIX "shaders/shadow_mapping.fs"};
    Shader depthShader{RESOURCE_PATH_PREFIX "shaders/shadow_mapping_depth.vs", RESOURCE_PATH_PREFIX "shaders/shadow_mapping_depth.fs"};
    //Shader debugDepthQuad{RESOURCE_PATH_PREFIX "shaders/debug_quad.vs", RESOURCE_PATH_PREFIX "shaders/debug_quad.fs"};

    shader.use();
    shader.setInt("diffuseTexture", 0); // 텍스쳐 이미지는 glActiveTexture(GL_TEXTURE0)
    shader.setInt("shadowMap", 1);      // 쉐이더에서 사용하는 그림자 맵은 glActiveTexture(GL_TEXTURE1)

    //debugDepthQuad.use();
    //debugDepthQuad.setInt("depthMap", 0);

    projection = glm::perspective(glm::radians(camera.Zoom), 1.0f * SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
    view = camera.GetViewMatrix();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region 1. Render Shadow map
        float near = 1.0f, far = 20.f;
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
        glm::mat4 light_view = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = light_projection * light_view;

        depthShader.use();
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glCullFace(GL_FRONT);
        renderScene(depthShader);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region 2. Render normally
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), 1.0f * SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
        view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);

        renderScene(shader);
#pragma endregion

#pragma region 3. Optionally render shadow map on a quad
        // debugDepthQuad.use();
        // debugDepthQuad.setFloat("near_plane", near);
        // debugDepthQuad.setFloat("far_plane", far);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        // renderQuad();
#pragma endregion

        if (showImGuiOverlay)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (ImGui::Begin("Bouncing ball", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Press F1 to toggle this overlay");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::Separator();
                if (ImGui::CollapsingHeader("Camera"))
                {
                    ImGui::BeginDisabled();
                    ImGui::InputFloat3("Position", glm::value_ptr(camera.Position));
                    ImGui::EndDisabled();
                }
                if (ImGui::CollapsingHeader("Light Settings"))
                {
                    ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
                    diffuseColor = lightColor * glm::vec3(0.8f);
                    ambientColor = diffuseColor * glm::vec3(0.2f);
                    ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPos), -3.0f, 3.0f, "%.3f");
                }
                if (ImGui::CollapsingHeader("Ball Debug", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::BeginDisabled();
                    ImGui::InputFloat3("Current Position", glm::value_ptr(ball_currentPos));
                    float norm = glm::length(ball_velocity);
                    ImGui::InputFloat("Velocity norm", &norm);
                    ImGui::EndDisabled();
                    ImGui::SliderFloat3("Initial Shooting Direction", glm::value_ptr(ball_initialShootingDirection), -1.0f, 1.0f, "%.3f");
                }
                ImGui::Separator();
                ImGui::SliderFloat("Gravity Strength", &gravity_strength, 0.01f, 10.0f);
                gravity = glm::vec3(0.0f, -gravity_strength, 0.0f);
                ImGui::SliderFloat("Drag", &drag, 0.999f, 0.8f, "%.3f", ImGuiSliderFlags_Logarithmic);
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void renderScene(const Shader &shader)
{
    glm::mat4 model = glm::identity<glm::mat4>();
    shader.use();

#pragma region Draw floor
    {
        model = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1.f));
        shader.setMat4("model", model);

        glBindVertexArray(woodFloorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
#pragma endregion

#pragma region Draw Light Cube(debug)
    {
        model = glm::scale(glm::translate(glm::identity<glm::mat4>(), lightPos), glm::vec3(0.2f));
        shader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lightCubeTexture);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
#pragma endregion

#pragma region Draw ball
    {
        auto drawBall = [&]() -> void
        {
            model = glm::translate(glm::identity<glm::mat4>(), ball_currentPos);
            model = glm::scale(model, ball_radius * glm::vec3(1.0f));
            shader.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ballTexture);

            glBindVertexArray(sphereVAO);
            glDrawArrays(GL_TRIANGLES, 0, nSphereVert);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
        };

        if (ball_hasStopped)
        {
            ball_stopTimer += deltaTime;
            LOG_INFO("Waiting... {:.2f}", ball_stopTimer);

            // Reset ball's properties
            if (ball_stopTimer > ball_stopTimerInitialValue)
            {
                LOG_INFO("Waiting done. Resetting ball.");
                ball_hasStopped = false;
                ball_stopTimer = 0.0f;
                ball_velocity = gravity_strength * ball_initialShootingDirection;
                ball_currentPos = ball_initialPos;
            }

            drawBall();
            return;
        }

        ball_velocity *= drag;

        ball_currentPos = ball_currentPos + ball_velocity * deltaTime + 0.5f * gravity * deltaTime * deltaTime;
        ball_velocity += gravity * deltaTime;

        bool isTowardGravity = glm::dot(ball_velocity, gravity) > 0.0f;
        // ball_radius가 반지름이 아니라 지름으로 적용되는 것 같아서 
        // 땅바닥으로부터 반지름 길이만큼 떠있는지 검사하기 위해 0.52f 곱함
        if (isTowardGravity && ball_currentPos.y <= ball_radius * 0.52f)
        {
            ball_velocity = 0.8f * glm::reflect(ball_velocity, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // Finished bouncing
        if (glm::length(ball_velocity) < 0.1f && ball_currentPos.y <= ball_radius * 0.52f)
        {
            LOG_INFO("Ball animation ended.");
            ball_currentPos.y = ball_radius * 0.52f;
            ball_hasStopped = true;
        }

        drawBall();
    }
#pragma endregion
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
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
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse || showImGuiOverlay)
        return;

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

void loadTexture(GLuint &textureID, const char *path)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    i32 width, height, nrChannels;
    u8 *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_ERROR("Failed to load texture at: {}", path);
    }
    stbi_image_free(data);
}

// initalize vertices of a sphere : position, normal, tex_coords.
// void initSphere(std::vector <float> data, int* nVert, int* nAttr)
void init_sphere(float **vertices, int *nVert, int *nAttr)
{
    //----------------------------------------
    // sphere: set up vertex data and configure vertex attributes
    float pi = acosf(-1.0f); // pi = 3.14152...
    float pi2 = 2.0f * pi;
    int nu = 40, nv = 20;
    const double du = pi2 / nu;
    const double dv = pi / nv;

    *nVert = (nv - 1) * nu * 6; // two triangles
    *nAttr = 8;
    *vertices = (float *)malloc(sizeof(float) * (*nVert) * (*nAttr));

    float u, v;
    int k = 0;

    v = 0.0f;
    u = 0.0f;
    for (v = (-0.5f) * pi + dv; v < 0.5f * pi - dv; v += dv)
    {
        for (u = 0.0f; u < pi2; u += du)
        {
            // p(u,v)
            (*vertices)[k++] = cosf(v) * cosf(u);
            (*vertices)[k++] = cosf(v) * sinf(u);
            (*vertices)[k++] = sinf(v); // position
            (*vertices)[k++] = cosf(v) * cosf(u);
            (*vertices)[k++] = cosf(v) * sinf(u);
            (*vertices)[k++] = sinf(v); // normal
            (*vertices)[k++] = u / pi2;
            (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords

            // p(u+du,v)
            (*vertices)[k++] = cosf(v) * cosf(u + du);
            (*vertices)[k++] = cosf(v) * sinf(u + du);
            (*vertices)[k++] = sinf(v); // position
            (*vertices)[k++] = cosf(v) * cosf(u + du);
            (*vertices)[k++] = cosf(v) * sinf(u + du);
            (*vertices)[k++] = sinf(v); // normal
            (*vertices)[k++] = (u + du) / pi2;
            (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords

            // p(u,v+dv)
            (*vertices)[k++] = cosf(v + dv) * cosf(u);
            (*vertices)[k++] = cosf(v + dv) * sinf(u);
            (*vertices)[k++] = sinf(v + dv); // position
            (*vertices)[k++] = cosf(v + dv) * cosf(u);
            (*vertices)[k++] = cosf(v + dv) * sinf(u);
            (*vertices)[k++] = sinf(v + dv); // normal
            (*vertices)[k++] = u / pi2;
            (*vertices)[k++] = (v + dv + 0.5f * pi) / pi; // texture coords

            // p(u+du,v)
            (*vertices)[k++] = cosf(v) * cosf(u + du);
            (*vertices)[k++] = cosf(v) * sinf(u + du);
            (*vertices)[k++] = sinf(v); // position
            (*vertices)[k++] = cosf(v) * cosf(u + du);
            (*vertices)[k++] = cosf(v) * sinf(u + du);
            (*vertices)[k++] = sinf(v); // normal
            (*vertices)[k++] = (u + du) / pi2;
            (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords

            // p(u+du,v+dv)
            (*vertices)[k++] = cosf(v + dv) * cosf(u + du);
            (*vertices)[k++] = cosf(v + dv) * sinf(u + du);
            (*vertices)[k++] = sinf(v + dv); // position
            (*vertices)[k++] = cosf(v + dv) * cosf(u + du);
            (*vertices)[k++] = cosf(v + dv) * sinf(u + du);
            (*vertices)[k++] = sinf(v + dv); // normal
            (*vertices)[k++] = (u + du) / pi2;
            (*vertices)[k++] = (v + dv + 0.5f * pi) / pi; // texture coords

            // p(u,v+dv)
            (*vertices)[k++] = cosf(v + dv) * cosf(u);
            (*vertices)[k++] = cosf(v + dv) * sinf(u);
            (*vertices)[k++] = sinf(v + dv); // position
            (*vertices)[k++] = cosf(v + dv) * cosf(u);
            (*vertices)[k++] = cosf(v + dv) * sinf(u);
            (*vertices)[k++] = sinf(v + dv); // normal
            (*vertices)[k++] = u / pi2;
            (*vertices)[k++] = (v + dv + 0.5f * pi) / pi; // texture coords
        }
    }
    // triangles around north pole and south pole
    for (u = 0.0f; u < pi2; u += du)
    {
        // triangles around north pole
        // p(u,pi/2-dv)
        v = 0.5f * pi - dv;
        (*vertices)[k++] = cosf(v) * cosf(u);
        (*vertices)[k++] = cosf(v) * sinf(u);
        (*vertices)[k++] = sinf(v); // position
        (*vertices)[k++] = cosf(v) * cosf(u);
        (*vertices)[k++] = cosf(v) * sinf(u);
        (*vertices)[k++] = sinf(v); // normal
        (*vertices)[k++] = u / pi2;
        (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords

        // p(u+du,pi/2-dv)
        v = 0.5f * pi - dv;
        (*vertices)[k++] = cosf(v) * cosf(u + du);
        (*vertices)[k++] = cosf(v) * sinf(u + du);
        (*vertices)[k++] = sinf(v); // position
        (*vertices)[k++] = cosf(v) * cosf(u + du);
        (*vertices)[k++] = cosf(v) * sinf(u + du);
        (*vertices)[k++] = sinf(v); // normal
        (*vertices)[k++] = (u + du) / pi2;
        (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords

        // p(u,pi/2) = (0, 1. 0)  ~ north pole
        v = 0.5f * pi;
        (*vertices)[k++] = cosf(v) * cosf(u + du);
        (*vertices)[k++] = cosf(v) * sinf(u + du);
        (*vertices)[k++] = sinf(v); // position
        (*vertices)[k++] = cosf(v) * cosf(u + du);
        (*vertices)[k++] = cosf(v) * sinf(u + du);
        (*vertices)[k++] = sinf(v); // normal
        (*vertices)[k++] = (u + du) / pi2;
        (*vertices)[k++] = 1.0f; // texture coords

        // triangles around south pole
        // p(u,-pi/2) = (0, -1, 0)  ~ south pole
        v = (-0.5f) * pi;
        (*vertices)[k++] = cosf(v) * cosf(u);
        (*vertices)[k++] = cosf(v) * sinf(u);
        (*vertices)[k++] = sinf(v); // position
        (*vertices)[k++] = cosf(v) * cosf(u);
        (*vertices)[k++] = cosf(v) * sinf(u);
        (*vertices)[k++] = sinf(v); // normal
        (*vertices)[k++] = u / pi2;
        (*vertices)[k++] = 0.0f; // texture coords

        // p(u+du,-pi/2+dv)
        v = (-0.5f) * pi + dv;
        (*vertices)[k++] = cosf(v) * cosf(u + du);
        (*vertices)[k++] = cosf(v) * sinf(u + du);
        (*vertices)[k++] = sinf(v); // position
        (*vertices)[k++] = cosf(v) * cosf(u + du);
        (*vertices)[k++] = cosf(v) * sinf(u + du);
        (*vertices)[k++] = sinf(v); // normal
        (*vertices)[k++] = (u + du) / pi2;
        (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords

        // p(u,-pi/2+dv)
        (*vertices)[k++] = cosf(v) * cosf(u);
        (*vertices)[k++] = cosf(v) * sinf(u);
        (*vertices)[k++] = sinf(v); // position
        (*vertices)[k++] = cosf(v) * cosf(u);
        (*vertices)[k++] = cosf(v) * sinf(u);
        (*vertices)[k++] = sinf(v); // normal
        (*vertices)[k++] = u / pi2;
        (*vertices)[k++] = (v + 0.5f * pi) / pi; // texture coords
    }
}