﻿#include "hzpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace Hazel
{
    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        // Create an empty vertex shader handle
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const char* source = vertexSrc.c_str();
        glShaderSource(vertexShader, 1, &source, 0);

        // Compile the vertex shader
        glCompileShader(vertexShader);

        GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<char> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(vertexShader);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Shader compilation failure");

            // In this simple program, we'll just leave
            return;
        }

        // Create an empty fragment shader handle
        const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Send the fragment shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        source = (const char*)fragmentSrc.c_str();
        glShaderSource(fragmentShader, 1, &source, nullptr);

        // Compile the fragment shader
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            int maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(fragmentShader);
            // Either of them. Don't leak shaders.
            glDeleteShader(vertexShader);

            // Use the infoLog as you see fit.
            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Shader compilation failure");

            return;
        }

        // Vertex and fragment shaders are successfully compiled.
        // Now time to link them together into a program.
        // Get a program object.
        const unsigned int program = glCreateProgram();
        m_RendererID = program;

        // Attach our shaders to our program
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        int isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE) {
            int maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            // Don't leak shaders either.
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            // Use the infoLog as you see fit.
            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "Shader linking failure");

            // In this simple program, we'll just leave
            return;
        }

        // Always detach shaders after a successful link.
        glDetachShader(program, vertexShader);
        glDetachShader(program, fragmentShader);
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const {
        glUseProgram(0);
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform3f(location, values.x, values.y, values.z);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform2f(location, values.x, values.y);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value)
    {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform1i(location, value);
    }

    /**
     * BIND BEFORE UPLOADING
     */
    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        HZ_CORE_ASSERT(location != -1, "Uniform does not exist!");
        glUniform4f(location, values.x, values.y, values.z, values.w);
    }
}