#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "_shader.h"
#include "camera.h"
#include "teapot_loader.h"
// #include "model.h"

#include <iostream>
#include <vector>

struct TeapotData { GLuint vao, vbo, nVertexNum; };
TeapotData g_teapotData {.vao = 0, .vbo = 0, };

struct SphereData { GLuint vao, vbo; int nSphereVert, nSphereAttr; };
SphereData g_sphereData { .vao = 0, .vbo = 0 };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLuint loadTexture(const char *path);
GLuint loadCubemap(std::vector<std::string> faces);
void init_sphere(float **vertices, int *nVert, int *nAttr);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2291012 YunHyeokNam", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader(RESOURCE_PATH_PREFIX "shaders/60.3.teapot.vs", RESOURCE_PATH_PREFIX "shaders/60.3.teapot.fs");
    Shader skyboxShader(RESOURCE_PATH_PREFIX "shaders/60.1.skybox.vs", RESOURCE_PATH_PREFIX "shaders/60.1.skybox.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float cubeVertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    std::vector<float> teapot_data{};
    Teapot teapot{RESOURCE_PATH_PREFIX "other/teapot.vbo", teapot_data, 8};
    g_teapotData.nVertexNum = teapot.nVertNum;
    glGenVertexArrays(1, &g_teapotData.vao);
    glGenBuffers(1, &g_teapotData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_teapotData.vbo);
    glBufferData(GL_ARRAY_BUFFER, teapot.nVertNum * teapot.nVertFloats * sizeof(float), &teapot_data[0],
                 GL_STATIC_DRAW);

    glBindVertexArray(g_teapotData.vao);
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
    glBindVertexArray(0);

    // innitialize sphere
    float *sphereVerts = nullptr;
    init_sphere(&sphereVerts, &g_sphereData.nSphereVert, &g_sphereData.nSphereAttr);
    glGenVertexArrays(1, &g_sphereData.vao);
    glBindVertexArray(g_sphereData.vao);
    {
        glGenBuffers(1, &g_sphereData.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_sphereData.vbo);
        glBufferData(GL_ARRAY_BUFFER, g_sphereData.nSphereVert * g_sphereData.nSphereAttr * sizeof(float), sphereVerts, GL_STATIC_DRAW);
        free(sphereVerts);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, g_sphereData.nSphereAttr * sizeof(float), (const GLvoid *)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, g_sphereData.nSphereAttr * sizeof(float), (const GLvoid *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texCoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, g_sphereData.nSphereAttr * sizeof(float), (const GLvoid *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);


    // cube VAO
    // GLuint cubeVAO, cubeVBO;
    // glGenVertexArrays(1, &cubeVAO);
    // glGenBuffers(1, &cubeVBO);
    // glBindVertexArray(cubeVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox VAO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------
    std::vector<std::string> faces
    {
        (RESOURCE_PATH_PREFIX "textures/skybox/right.jpg"),
        (RESOURCE_PATH_PREFIX "textures/skybox/left.jpg"),
        (RESOURCE_PATH_PREFIX "textures/skybox/top.jpg"),
        (RESOURCE_PATH_PREFIX "textures/skybox/bottom.jpg"),
        (RESOURCE_PATH_PREFIX "textures/skybox/front.jpg"),
        (RESOURCE_PATH_PREFIX "textures/skybox/back.jpg")
    };
    GLuint cubemapTexture = loadCubemap(faces);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("skybox", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene as normal
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("eyePos", camera.Position);

        model = glm::identity<glm::mat4>();
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        // draw teapot
        glBindVertexArray(g_teapotData.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, g_teapotData.nVertexNum);
        glBindVertexArray(0); 

        // draw sphere 1
        model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.3f));
        shader.setMat4("model", model);
        glBindVertexArray(g_sphereData.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, g_sphereData.nSphereVert);
        glBindVertexArray(0);

        // draw sphere 2
        model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f));
        shader.setMat4("model", model);
        glBindVertexArray(g_sphereData.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, g_sphereData.nSphereVert);
        glBindVertexArray(0);

        // draw sphere 3
        model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-1.0f, -0.2f, 0.3f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        shader.setMat4("model", model);
        glBindVertexArray(g_sphereData.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, g_sphereData.nSphereVert);
        glBindVertexArray(0);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        {
            skyboxShader.use();
            view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);
            // skybox cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);    
        }
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &g_sphereData.vao);
    glDeleteBuffers(1, &g_sphereData.vbo);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
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
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
GLuint loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
GLuint loadCubemap(std::vector<std::string> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (std::size_t i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
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