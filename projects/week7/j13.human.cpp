//////////////////////////////////////////
//		jieunlee@hansung.ac.kr			//
//		2020. 10. 12					//
//////////////////////////////////////////

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdio>

#include "_shader.h"
#include "camera.h"
#include "j13.human.h"
#include "AnimationState.h"

#define ERROR(fmt, ...)                                                                            \
    do                                                                                             \
    {                                                                                              \
        std::fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__);                                      \
    } while (0)

#ifndef RESOURCE_PATH_PREFIX
#define RESOURCE_PATH_PREFIX ""
#endif

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

bool isF1KeyPressed = false;
bool showImGuiOverlay = true;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 10.0f, 20.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2291012 YunHyeok Nam", nullptr, nullptr);
    if (window == nullptr)
    {
        ERROR("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ERROR("Failed to initialize GLAD\n");
        return -1;
    }
    glfwSwapInterval(1);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader boneShader(RESOURCE_PATH_PREFIX "j13.human.vs",
                      RESOURCE_PATH_PREFIX "j13.human.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, 0.0f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  0.0f, -0.5f, 0.0f,  0.0f,  -1.0f,
        0.5f,  1.0f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  1.0f, -0.5f, 0.0f,  0.0f,  -1.0f,
        -0.5f, 1.0f, -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.0f, -0.5f, 0.0f,  0.0f,  -1.0f,

        -0.5f, 0.0f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.0f, 0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  1.0f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  1.0f, 0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, 1.0f, 0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.0f, 0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f, 1.0f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 1.0f, -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, 0.0f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, 0.0f, -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, 0.0f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 1.0f, 0.5f,  -1.0f, 0.0f,  0.0f,

        0.5f,  1.0f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  1.0f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  0.0f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  0.0f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  0.0f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  1.0f, 0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, 0.0f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  0.0f, -0.5f, 0.0f,  -1.0f, 0.0f,
        0.5f,  0.0f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.5f,  0.0f, 0.5f,  0.0f,  -1.0f, 0.0f,
        -0.5f, 0.0f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, 0.0f, -0.5f, 0.0f,  -1.0f, 0.0f,

        -0.5f, 1.0f, -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  1.0f, -0.5f, 0.0f,  1.0f,  0.0f,
        0.5f,  1.0f, 0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f, 0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f, 1.0f, 0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 1.0f, -0.5f, 0.0f,  1.0f,  0.0f};

    // first, configure the cube's VAO (and VBO)
    GLuint VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the
    // light object which is also a 3D cube)
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Human
    Human human{};
    Human_Pose currentPose{walk_1}, nextPose{walk_2};

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

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
        boneShader.use();
        boneShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        boneShader.setVec3("lightPos", lightPos);
        boneShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        boneShader.setMat4("projection", projection);
        boneShader.setMat4("view", view);

        // world transformation
        static AnimationController animationController{};
        glm::mat4 model = glm::mat4(1.0f);
        static glm::mat4 lastModel = model;
        static float s = 0.0f;
        if (animationController.walkingCycleCount < animationController.MAX_WALKING_CYCLES)
        {
            s += deltaTime;
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, s));
            lastModel = model;
        }
        else
        {
            model = lastModel;
        }

        boneShader.setMat4("model", model);

        // render a human
        // human.SetBoneRotation(upperarmL, glm::angleAxis(glm::radians(30.f), glm::vec3(0.f,
        // 0.f, 1.f))); human.SetBoneRotation(forearmL, glm::angleAxis(glm::radians(60.f),
        // glm::vec3(0.f, 0.f, 1.f))); human.SetPose(armLeftUp);
        static float t = 0.0f;
        static float animationDuration = 1.0f;
        static std::vector<glm::quat> BoneSnapshot;
        float dt = deltaTime;
        human.DrawHuman(boneShader, cubeVAO, model);
        // human.MixPose(currentPose, nextPose, t);
        if ((animationController.currentState == ANIM_GREETING) || 
            (animationController.currentState == ANIM_WALKING))
        {
            human.MixPose(currentPose, nextPose, t);
        }
        else
        {
            human.MixPose(BoneSnapshot, nextPose, t);
        }
        t = t + (dt / animationDuration);
        static bool greetingStarted = false;
        if (t > 1.0f)
        {
            t = 0.0f;

            switch (animationController.currentState)
            {
            case ANIM_WALKING:
            {
                switch (nextPose)
                {
                case walk_2:
                {
                    currentPose = walk_2;
                    nextPose = walk_3;
                    animationDuration = 0.9f;
                }
                break;

                case walk_3:
                {
                    currentPose = walk_3;
                    nextPose = walk_4;
                    animationDuration = 0.5f;
                }
                break;

                case walk_4:
                {
                    currentPose = walk_4;
                    nextPose = walk_1;
                    animationDuration = 0.9f;
                }
                break;

                case walk_1:
                {
                    animationController.walkingCycleCount++;
                    currentPose = walk_1;
                    nextPose = walk_2;
                    animationDuration = 0.5f;

                    if (animationController.walkingCycleCount >=
                        animationController.MAX_WALKING_CYCLES)
                    {
                        animationController.currentState = ANIM_WALKING_TO_GREETING;
                        human.CloneCurrentBoneRotation(BoneSnapshot);
                        nextPose = greet_0;
                        animationDuration = 1.5f;
                    }
                }
                break;

                default:
                    break;
                }
            }
            break;

            case ANIM_WALKING_TO_GREETING:
            {
                currentPose = nextPose;
                nextPose = greet_1;
                animationController.currentState = ANIM_GREETING;
                animationController.greetingCycleCount = 0;
                animationDuration = 1.0f;
            }
            break;

            case ANIM_GREETING:
            {
                switch (nextPose)
                {
                case greet_1:
                {
                    currentPose = greet_1;
                    nextPose = greet_2;
                    animationDuration = 0.3f;
                }
                break;

                case greet_2:
                {
                    currentPose = greet_2;
                    nextPose = greet_3;
                    animationDuration = 0.4f;
                }
                break;

                case greet_3:
                {
                    currentPose = greet_3;
                    nextPose = greet_4;
                    animationDuration = 0.3f;
                }
                break;

                case greet_4:
                {
                    currentPose = greet_4;
                    nextPose = greet_2;
                    animationController.greetingCycleCount++;
                    animationDuration = 0.4f;

                    // Check if we should transition back to walking
                    if (animationController.greetingCycleCount >=
                        animationController.MAX_GREETING_CYCLES)
                    {
                        human.CloneCurrentBoneRotation(BoneSnapshot);
                        animationController.currentState = ANIM_GREETING_TO_WALKING;
                        nextPose = walk_1;
                        animationDuration = 1.f;
                    }
                }
                break;

                default:
                    break;
                }
            }
            break;

            case ANIM_GREETING_TO_WALKING:
            {
                currentPose = nextPose;
                nextPose = walk_2;
                animationController.currentState = ANIM_WALKING;
                animationController.walkingCycleCount = 0;
                animationDuration = 0.5f;
            }
            break;
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
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