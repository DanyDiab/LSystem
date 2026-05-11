#include "./headers/Camera.hpp"
#include <cmath>

namespace LSystem {

    Camera::Camera() 
        : pos(0.0f, 100.0f, 300.0f),
          front(0.0f, 0.0f, -1.0f),
          up(0.0f, 1.0f, 0.0f),
          yaw(-90.0f),
          pitch(-15.0f),
          zoom(45.0f),
          lastX(500.0f),
          lastY(500.0f) {
        updateCameraVectors();
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(pos, pos + front, up);
    }

    glm::mat4 Camera::getProjectionMatrix(float aspect) {
        return glm::perspective(glm::radians(zoom), aspect, 0.1f, 10000.0f);
    }

    void Camera::processKeyboard(unsigned char key, float deltaTime) {
        float cameraSpeed = 150.0f * deltaTime;
        if (key == 'w' || key == 'W') {
            pos += cameraSpeed * front;
        }
        if (key == 's' || key == 'S') {
            pos -= cameraSpeed * front;
        }
        if (key == 'a' || key == 'A') {
            pos -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
        }
        if (key == 'd' || key == 'D') {
            pos += glm::normalize(glm::cross(front, up)) * cameraSpeed;
        }
        if (key == 'q' || key == 'Q') {
            pos -= up * cameraSpeed;
        }
        if (key == 'e' || key == 'E') {
            pos += up * cameraSpeed;
        }
    }

    void Camera::processMouse(float xOffset, float yOffset) {
        float sensitivity = 0.15f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        updateCameraVectors();
    }

    void Camera::updateCameraVectors() {
        if (pitch > 89.0f) { pitch = 89.0f; }
        if (pitch < -89.0f) { pitch = -89.0f; }

        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
    }
}
