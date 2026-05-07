#include "./headers/Renderer.hpp"
#include <iostream>

namespace LSystem {

    Renderer::Renderer() : vao(0), vbo(0), instancesVbo(0), vertexCount(0), instanceCount(0) {}

    void Renderer::setupMesh(const std::vector<float>& meshData) {
        if (meshData.empty()) {
            return;
        }

        if (vao == 0) {
            glGenVertexArrays(1, &vao);
        }
        if (vbo == 0) {
            glGenBuffers(1, &vbo);
        }

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(float), meshData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        vertexCount = static_cast<int>(meshData.size() / 3);
    }

    void Renderer::updateInstances(const std::vector<glm::vec3>& positions) {
        if (positions.empty()) {
            return;
        }

        if (vao == 0) {
            return;
        }

        if (instancesVbo == 0) {
            glGenBuffers(1, &instancesVbo);
        }

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, instancesVbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glVertexAttribDivisor(1, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        instanceCount = static_cast<int>(positions.size());
    }

    void Renderer::draw(Shader& shader, Camera& camera, int width, int height) {
        if (vao == 0 || instanceCount == 0) {
            return;
        }

        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        glm::mat4 projection = camera.getProjectionMatrix(aspect);

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertexCount, instanceCount);
        glBindVertexArray(0);
    }
}
