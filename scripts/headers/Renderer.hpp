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
        int vertexCount;
        int instanceCount;

        Renderer();
        void setupMesh(const std::vector<float>& meshData);
        void updateInstances(const std::vector<glm::vec3>& positions);
        void draw(Shader& shader, Camera& camera, int width, int height);
    };
}
