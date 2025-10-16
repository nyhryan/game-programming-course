#pragma once

#include <glad/glad.h>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <string>

namespace GameProgramming::Shader
{

enum class ShaderType
{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER
};

class ShaderProgram
{
public:
    ShaderProgram(std::filesystem::path vertexShaderSrc = "./shader.vert", std::filesystem::path fragmentShaderSrc = "./shader.frag");
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    void use() const noexcept { glUseProgram(m_program); }

    [[nodiscard]] GLuint get() const noexcept { return m_program; }
    [[nodiscard]] GLint getUniformLocation(const char *uniformName) const noexcept { return glGetUniformLocation(m_program, uniformName); }

    void setUniformMatrix4f(const char *uniformName, const GLfloat *matrix) const noexcept
    {
        glUniformMatrix4fv(glGetUniformLocation(m_program, uniformName), 1, GL_FALSE, matrix);
    }

    void setUniformMatrix4f(const char *uniformName, const glm::mat4 &matrix) const noexcept
    {
        glUniformMatrix4fv(glGetUniformLocation(m_program, uniformName), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void setUniformVec3(const char *uniformName, const GLfloat *vector) const noexcept
    {
        glUniform3fv(glGetUniformLocation(m_program, uniformName), 1, vector);
    }

    void setUniformVec3(const char *uniformName, const glm::vec3 &vector) const noexcept
    {
        glUniform3fv(glGetUniformLocation(m_program, uniformName), 1, glm::value_ptr(vector));
    }

    void setUniformVec3(const char *uniformName, GLfloat x, GLfloat y, GLfloat z) const noexcept
    {
        glUniform3f(glGetUniformLocation(m_program, uniformName), x, y, z);
    }

    void setUniformFloat(const char *uniformName, const GLfloat value) const noexcept
    {
        glUniform1f(glad_glGetUniformLocation(m_program, uniformName), value);
    }

private:
    GLuint m_program;
};

class ShaderObject
{
public:
    ShaderObject(std::filesystem::path src, ShaderType shaderType);
    ~ShaderObject();
    ShaderObject(const ShaderObject &) = delete;
    ShaderObject operator=(const ShaderObject &) = delete;
    ShaderObject(ShaderObject &&) = delete;
    ShaderObject operator=(ShaderObject &&) = delete;

    [[nodiscard]] GLuint getID() const noexcept
    {
        return m_shader;
    }

private:
    GLuint m_shader;
    std::filesystem::path m_shaderPath;
};

class ShaderLoader
{
public:
    static std::string loadShaderScript(const std::filesystem::path &src);
};

} // namespace GameProgramming::Shader