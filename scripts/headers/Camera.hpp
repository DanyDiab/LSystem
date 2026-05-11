#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace LSystem {
    class Camera {
    public:
        glm::vec3 pos;
        glm::vec3 front;
        glm::vec3 up;
        float yaw;
        float pitch;
        float zoom;
        float lastX;
        float lastY;
        
        Camera();

        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix(float aspect);
        void processKeyboard(unsigned char key, float deltaTime);
        void processMouse(float xOffset, float yOffset);
    private:
        void updateCameraVectors();
    };
}
