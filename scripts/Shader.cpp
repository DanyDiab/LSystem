#include "./headers/Shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace LSystem {

    Shader::Shader() : ID(0) {}

    void Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
        auto readFile = [](const std::string& path) -> std::string {
            std::ifstream file(path);
            if (!file.is_open()) {
                std::cerr << "Failed to open shader file: " << path << std::endl;
                return "";
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        };

        std::string vertexCode = readFile(vertexPath);
        std::string fragmentCode = readFile(fragmentPath);

        if (vertexCode.empty() || fragmentCode.empty()) {
            return;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        
        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            std::cerr << "Vertex shader compilation failed" << std::endl;
            return;
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            std::cerr << "Fragment shader compilation failed" << std::endl;
            return;
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);
        
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            std::cerr << "Shader program linking failed" << std::endl;
            return;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shader::use() {
        if (ID != 0) {
            glUseProgram(ID);
        }
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& matrix) {
        if (ID == 0) return;
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
}
