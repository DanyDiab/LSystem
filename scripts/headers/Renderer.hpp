#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "./Shader.hpp"
#include "./Camera.hpp"

namespace LSystem {
    class Renderer {
    public:
        GLuint vao;
        GLuint vbo;
        GLuint instancesVbo;
        GLuint widthsVbo;
        
        int vertexCount;
        int instanceCount;

        Renderer();
        void setupMesh(const std::vector<float>& meshData);
        void updateInstances(const std::vector<glm::mat4>& models, const std::vector<float>& widths);
        void draw(Shader& shader, Camera& camera, int width, int height);
    };
}
