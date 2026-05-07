#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include "./headers/Camera.hpp"
#include "./headers/Shader.hpp"
#include "./headers/Renderer.hpp"
#include "./headers/Geometry.hpp"
#include "./headers/turtle.hpp"

using namespace std;

LSystem::Camera* camera = nullptr;
LSystem::Shader* shader = nullptr;
LSystem::Renderer* renderer = nullptr;

int windowWidth = 1000;
int windowHeight = 1000;
int lastFrameTime = 0;
bool keys[256] = {false};
bool appRunning = true;

void init() {
    glewInit();
    
    camera = new LSystem::Camera();
    shader = new LSystem::Shader();
    renderer = new LSystem::Renderer();

    shader->load("./shaders/vert.vert", "./shaders/frag.frag");

    readInJSON();
    std::vector<float> points = executeInstructions();

    std::vector<glm::vec3> instances;
    for (size_t i = 0; i < points.size(); i += 3) {
        instances.push_back(glm::vec3(points[i], points[i+1], points[i+2]));
    }

    std::vector<float> mesh = LSystem::Geometry::generateCylinder(20, 3.0f, 5.0f);
    renderer->setupMesh(mesh);
    renderer->updateInstances(instances);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_NONE);
    glutSetCursor(GLUT_CURSOR_NONE);
}

void display() {
    if (!appRunning) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (renderer && shader && camera) {
        renderer->draw(*shader, *camera, windowWidth, windowHeight);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void update() {
    if (!appRunning) return;

    int currentFrameTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentFrameTime;

    if (camera) {
        for (int i = 0; i < 256; i++) {
            if (keys[i]) {
                camera->processKeyboard((unsigned char)i, deltaTime);
            }
        }
    }
}

void keyboardDown(unsigned char key, int x, int y) {
    keys[key] = true;
    if (key == 27) {
        appRunning = false;
        exit(0);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouseMove(int x, int y) {
    if (!camera || !appRunning) return;

    static bool firstMouse = true;
    static bool isWarping = false;
    
    if (isWarping) {
        isWarping = false;
        return;
    }

    if (firstMouse) {
        camera->lastX = static_cast<float>(x);
        camera->lastY = static_cast<float>(y);
        firstMouse = false;
    }

    float xOffset = static_cast<float>(x) - camera->lastX;
    float yOffset = camera->lastY - static_cast<float>(y);

    camera->lastX = static_cast<float>(x);
    camera->lastY = static_cast<float>(y);

    camera->processMouse(xOffset, yOffset);

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    if (x != centerX || y != centerY) {
        isWarping = true;
        glutWarpPointer(centerX, centerY);
        camera->lastX = static_cast<float>(centerX);
        camera->lastY = static_cast<float>(centerY);
    }
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (!camera || !appRunning) return;
    
    if (direction > 0) {
        camera->zoom -= 2.0f;
    } else if (direction < 0) {
        camera->zoom += 2.0f;
    }

    if (camera->zoom < 1.0f) camera->zoom = 1.0f;
    if (camera->zoom > 120.0f) camera->zoom = 120.0f;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("L-System 3D");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMove);
    glutMouseWheelFunc(mouseWheel);

    glutMainLoop();
    return 0;
}
