//////////////////////////////////////////
//		jieunlee@hansung.ac.kr			//
//		Solar system (Earth)			//
//		2023. 5. 11						//
//////////////////////////////////////////
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void init_sphere(float **, int *, int *);
void init_textures();

#define TEXTURE_PATH_PREFIX ""
// #define TEXTURE_PATH_PREFIX "..\\textures\\solarsystem\\"

// settings
constexpr u32 SCR_WIDTH = 1000;
constexpr u32 SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(21.2f, 21.0f, 12.0f);

// the sun, planets, and moon
// 자전주기(일): 태양 25.38일, 수성 58.6462, 금성	-243.0185, 지구	0.99726963,	(달 27.3216)
// 화성은 1.02595676, 목성은 0.410일, 토성은 0.426일, 천왕성은 0.718일, 해왕성은 0.669일
// 공전주기(일): 수성 87.97, 금성 224.7, 지구 365.26, (달 27.322) 화성 686.96, 목성 4333.29, 토성
// 10,756.20,	 천왕성 30,707.49, 	해왕성 60,223.35 rotation periods of the sun, planets, and moon
const float rotp_sun = 25.38f;
const float rotp_mercury = 58.6462f;
const float rotp_venus = 243.0185f;
// const float rotp_earth = 0.99726963f;
// const float rotp_moon = 27.3216f;
// const float rotp_mars = 1.02595676f;
// const float rotp_jupiter = 0.410f;
// const float rotp_saturn = 0.426f;
// const float rotp_uranus = 0.718f;
// const float rotp_neptune = 0.669;

// revolution periods of the planets, and moon
const float revp_mercury = 87.97f;
const float revp_venus = 224.7f;
// const float revp_earth = 365.26f;
// const float revp_moon = 27.322f;
// const float revp_mars = 686.96f;
// const float revp_jupiter = 4333.29f;
// const float revp_saturn = 10756.20f;
// const float revp_uranus = 30707.49f;
// const float revp_neptune = 60223.35;

// speed of rotation and revolution
const float rot_speed = 20.0f;

// scales of the planets, and moon
const float radi_sun = 6.0f;      // 696340.0f;
const float radi_mercury = 0.24f; // 2439.7f;
const float radi_venus = 0.60f;   // 6051.8f;
// const float radi_earth = 0.63f;	//6371.0f;
// const float radi_moon = 0.17f;	//1737.4f;
// const float radi_mars = 0.3389f;	//3389.5f;
// const float radi_jupiter = 1.0f; // 69911.0f;
// const float radi_saturn = 0.9f; // 58232.0f;
// const float radi_uranus = 0.43f; // 25362.0f;
// const float radi_neptune = 0.4f; // 24622.0f;

// textures
GLuint texture_sun, texture_mercury, texture_venus, texture_earth, texture_moon;
GLuint texture_mars, texture_jupiter, texture_saturn, texture_uranus, texture_neptune;

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    GameProgramming::Shader::ShaderProgram _planetShader{"solarsystem_planet.vs",
                                                         "solarsystem_planet.fs"};

    // Shader planetShader("solarsystem_color.vs", "solarsystem_color.fs");
    GameProgramming::Shader::ShaderProgram _starShader{"solarsystem_star.vs",
                                                       "solarsystem_star.fs"};

    // sphere VAO and VBO
    // std::vector <float> data;
    float *sphereVerts = nullptr;
    int nSphereVert, nSphereAttr;
    init_sphere(&sphereVerts, &nSphereVert, &nSphereAttr);

    GLuint sphereVBO, sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, nSphereVert * nSphereAttr * sizeof(float), sphereVerts,
                 GL_STATIC_DRAW);

    glBindVertexArray(sphereVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nSphereAttr * sizeof(float), (const GLvoid *)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nSphereAttr * sizeof(float),
                          (const GLvoid *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, nSphereAttr * sizeof(float),
                          (const GLvoid *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    free(sphereVerts);

    // init textures
    init_textures();

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float f = ImGui::GetFontSize();
        ImGui::SetNextWindowSize({f * 10.f, f * 10.f});
        ImGui::Begin("Solar System!");
        ImGui::End();

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

        // sun
        // -----------
        // be sure to activate shader when setting uniforms/drawing objects
        _starShader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), 1.0f * SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        _starShader.setUniformMatrix4f("projection", projection);
        _starShader.setUniformMatrix4f("view", view);
        // eye position
        _starShader.setUniformVec3("eyePos", camera.Position);

        // draw the sphere object
        // light properties
        glm::vec3 lightColor(1.0, 1.0, 1.0);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f);   // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
        _starShader.setUniformVec3("light.ambient", ambientColor);
        _starShader.setUniformVec3("light.diffuse", diffuseColor);
        _starShader.setUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);
        _starShader.setUniformVec3("light.position", lightPos);
        // material properties
        _starShader.setUniformVec3("material.ambient", 0.5f, 0.5f, 0.5f);
        _starShader.setUniformVec3("material.diffuse", 0.9f, 0.9f, 0.9f);
        _starShader.setUniformVec3(
            "material.specular", 0.5f, 0.5f,
            0.5f); // specular lighting doesn't have full effect on this object's material
        _starShader.setUniformFloat("material.shininess", 20.0f);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 sun_model = model;
        model = glm::rotate(model, (float)glfwGetTime() * rot_speed / rotp_sun,
                            glm::vec3(0.0f, 1.0f, 0.0f)); // the rotation of the sun
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(radi_sun, radi_sun, radi_sun));
        _starShader.setUniformMatrix4f("model", model);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_sun);

        // render the sphere
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, nSphereVert);

        // Planets, moon
        // -----------
        // be sure to activate shader when setting uniforms/drawing objects
        _planetShader.use();
        // view/projection transformations
        _planetShader.setUniformMatrix4f("projection", projection);
        _planetShader.setUniformMatrix4f("view", view);
        // eye position
        _planetShader.setUniformVec3("eyePos", camera.Position);

        // draw the sphere object
        // light properties
        // glm::vec3 lightColor(1.0, 1.0, 1.0);;
        // glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f); // decrease the influence
        // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
        _planetShader.setUniformVec3("light.ambient", ambientColor);
        _planetShader.setUniformVec3("light.diffuse", diffuseColor);
        _planetShader.setUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);
        _planetShader.setUniformVec3("light.position", lightPos);
        // material properties
        _planetShader.setUniformVec3("material.ambient", 0.5f, 0.5f, 0.5f);
        _planetShader.setUniformVec3("material.diffuse", 0.9f, 0.9f, 0.9f);
        _planetShader.setUniformVec3(
            "material.specular", 0.5f, 0.5f,
            0.5f); // specular lighting doesn't have full effect on this object's material
        _planetShader.setUniformFloat("material.shininess", 20.0f);

        // mercury
        // -----------
        // world transformation
        float dist = radi_sun + 3 * radi_mercury;
        model = sun_model;
        model = glm::rotate(model, (float)glfwGetTime() * rot_speed / revp_mercury,
                            glm::vec3(0.0f, 1.0f, 0.0f)); // the revolution of the earth
        model = glm::translate(
            model, glm::vec3(dist, 0.0f, 0.0f)); // the translation of the earth from the sun
        model = glm::rotate(model, (float)glfwGetTime() * rot_speed / rotp_mercury,
                            glm::vec3(0.0f, 1.0f, 0.0f)); // the rotation of the earth
        model = glm::scale(model, glm::vec3(radi_mercury, radi_mercury, radi_mercury));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        _planetShader.setUniformMatrix4f("model", model);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_mercury);

        // render the sphere
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, nSphereVert);

        // venus
        // -----------
        // world transformation
        dist = dist + 3 * radi_venus;
        model = sun_model;
        model = glm::rotate(model, (float)glfwGetTime() * rot_speed / revp_venus,
                            glm::vec3(0.0f, 1.0f, 0.0f)); // the revolution of the earth
        model = glm::translate(
            model, glm::vec3(dist, 0.0f, 0.0f)); // the translation of the earth from the sun
        model = glm::rotate(model, (float)glfwGetTime() * rot_speed / rotp_venus,
                            glm::vec3(0.0f, 1.0f, 0.0f)); // the rotation of the earth
        model = glm::scale(model, glm::vec3(radi_venus, radi_venus, radi_venus));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        _planetShader.setUniformMatrix4f("model", model);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_venus);

        // render the sphere
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, nSphereVert);

        // earth
        // -----------

        // moon
        // -----------

        // mars
        // -----------

        // jupiter
        // -----------

        // saturn
        // -----------

        // uranus
        // -----------

        // neptune
        // -----------

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
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
    stbi_set_flip_vertically_on_load(
        true); // tell stb_image.h to flip loaded texture's on the y-axis.
    i32 width, height, nrChannels;
    u8 *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_ERROR("Failed to load texture");
    }
    stbi_image_free(data);
}

void init_textures()
{
    loadTexture(texture_sun,        TEXTURE_PATH_PREFIX "2k_sun.jpg");
    loadTexture(texture_earth,      TEXTURE_PATH_PREFIX "2k_earth_daymap.jpg");
    loadTexture(texture_moon,       TEXTURE_PATH_PREFIX "2k_moon.jpg");
    loadTexture(texture_mercury,    TEXTURE_PATH_PREFIX "2k_mercury.jpg");
    loadTexture(texture_venus,      TEXTURE_PATH_PREFIX "2k_venus_surface.jpg");
    loadTexture(texture_mars,       TEXTURE_PATH_PREFIX "2k_mars.jpg");
    loadTexture(texture_jupiter,    TEXTURE_PATH_PREFIX "2k_jupiter.jpg");
    loadTexture(texture_saturn,     TEXTURE_PATH_PREFIX "2k_saturn.jpg");
    loadTexture(texture_uranus,     TEXTURE_PATH_PREFIX "2k_uranus.jpg");
    loadTexture(texture_neptune,    TEXTURE_PATH_PREFIX "2k_neptune.jpg");
}