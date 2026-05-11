#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace LSystem {
    class Shader {
    public:
        GLuint ID;

        Shader();
        void load(const std::string& vertexPath, const std::string& fragmentPath);
        void use();
        void setMat4(const std::string& name, const glm::mat4& matrix);
    };
}
