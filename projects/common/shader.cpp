#include "shader.hpp"

#include "logger.hpp"
#include "utility.hpp"

#include <cassert>

#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace GameProgramming::Shader
{

ShaderProgram::ShaderProgram(std::filesystem::path vertexShaderSrc, std::filesystem::path fragmentShaderSrc)
    : m_program(glCreateProgram())
{
    ShaderObject vertexShader{vertexShaderSrc, ShaderType::Vertex};
    ShaderObject fragmentShader{fragmentShaderSrc, ShaderType::Fragment};
    glAttachShader(m_program, vertexShader.getID());
    glAttachShader(m_program, fragmentShader.getID());
    glLinkProgram(m_program);

    GLint isLinked{};
    glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);
    if (isLinked != GL_TRUE)
    {
        GLsizei len{};
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);
        std::vector<GLchar> buffer(len);
        glGetProgramInfoLog(m_program, len, &len, buffer.data());
        LOG_ERROR("Failed to link shader program: {}", buffer.data());
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_program);
}

ShaderObject::ShaderObject(std::filesystem::path src, ShaderType shaderType)
    : m_shader(0), m_shaderPath(src)
{
    m_shader = glCreateShader(Utility::to_underlying(shaderType));
    if (0 == m_shader)
    {
        std::string msg{"Failed to create shader object with: " +
                        std::filesystem::canonical(src).string()};
        LOG_ERROR("{}", msg);
        throw std::runtime_error{msg};
    }

    std::string shaderSource = ShaderLoader::loadShaderScript(src);
    const GLchar *shaderSource_cstr = shaderSource.c_str();
    glShaderSource(m_shader, 1, &shaderSource_cstr, nullptr);

    glCompileShader(m_shader);
    GLint isSuccess;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &isSuccess);
    if (isSuccess != GL_TRUE)
    {
        GLsizei len{};
        glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<GLchar> buffer(len);
        glGetShaderInfoLog(m_shader, len, &len, buffer.data());
        LOG_ERROR("Failed to compile shader: {}", buffer.data());
    }
}

ShaderObject::~ShaderObject()
{
    glDeleteShader(m_shader);
}

std::string ShaderLoader::loadShaderScript(const std::filesystem::path &src)
{
    std::filesystem::path canonical_path = std::filesystem::canonical(src);
    std::ifstream shaderFile{canonical_path};
    if (!shaderFile.is_open())
    {
        throw std::runtime_error{"Failed to open shader file: " +
                                 canonical_path.filename().string()};
    }

    std::stringstream ss{};
    ss << shaderFile.rdbuf();
    return ss.str();
}

} // namespace GameProgramming::Shader