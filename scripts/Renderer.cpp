#include "./headers/Renderer.hpp"
#include <iostream>

namespace LSystem {

    Renderer::Renderer() : vao(0), vbo(0), instancesVbo(0), widthsVbo(0), vertexCount(0), instanceCount(0) {}

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

    void Renderer::updateInstances(const std::vector<glm::mat4>& models, const std::vector<float>& widths) {
        if (models.empty() || widths.empty() || vao == 0) {
            return;
        }

        if (instancesVbo == 0) {
            glGenBuffers(1, &instancesVbo);
        }
        if (widthsVbo == 0) {
            glGenBuffers(1, &widthsVbo);
        }

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, instancesVbo);
        glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
        
        std::size_t vec4Size = sizeof(glm::vec4);

        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(1 + i);
            glVertexAttribPointer(i + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
            glVertexAttribDivisor(1 + i, 1);         
        }

        glBindBuffer(GL_ARRAY_BUFFER, widthsVbo);
        glBufferData(GL_ARRAY_BUFFER, widths.size() * sizeof(float), widths.data(), GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
        glVertexAttribDivisor(5, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        instanceCount = static_cast<int>(models.size());
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
