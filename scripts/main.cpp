#include <cstddef>
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
bool specialKeys[256] = {false};
bool appRunning = true;

void init() {
    glewInit();
    
    camera = new LSystem::Camera();
    shader = new LSystem::Shader();
    renderer = new LSystem::Renderer();

    shader->load("./shaders/vert.vert", "./shaders/frag.frag");

    std::tuple<std::vector<glm::mat4>, std::vector<float>> instaceInfo = executeInstructions();

    std::vector<glm::mat4> models = std::get<0>(instaceInfo);
    std::vector<float> widths = std::get<1>(instaceInfo);

    std::vector<float> mesh = LSystem::Geometry::generateCylinder(20, 1.0f, 1.0f);
    renderer->setupMesh(mesh);
    renderer->updateInstances(models, widths);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_NONE);
    glutSetCursor(GLUT_CURSOR_NONE);
}

void display() {
    if (!appRunning) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (renderer != NULL && shader != nullptr && camera != nullptr) {
        renderer->draw(*shader, *camera, windowWidth, windowHeight);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void update() {
    if (!appRunning) {
        return;
    }

    int currentFrameTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentFrameTime;

    if (camera == nullptr) {
        return;
    }
    for (int i = 0; i < 256; i++) {
        if (keys[i]) {
            camera->processKeyboard((unsigned char)i, deltaTime);
        }
        if (specialKeys[i]) {
            camera->processSpecialKeyboard(i, deltaTime);
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

void specialDown(int key, int x, int y) {
    if (key >= 0 && key < 256) {
        specialKeys[key] = true;
    }
}

void specialUp(int key, int x, int y) {
    if (key >= 0 && key < 256) {
        specialKeys[key] = false;
    }
}

void mouseMove(int x, int y) {
    if (camera == nullptr|| !appRunning) {
        return;
    }

    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    camera->processMouseMovement(xpos, ypos);
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (camera == nullptr || !appRunning) {
        return;
    }
    
    if (direction > 0) {
        camera->zoom -= 2.0f;
    } else if (direction < 0) {
        camera->zoom += 2.0f;
    }

    if (camera->zoom < 1.0f) {
        camera->zoom = 1.0f;
    }
    if (camera->zoom > 120.f) {
        camera->zoom = 120.0f;
    }
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
    
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    
    glutPassiveMotionFunc(mouseMove);
    glutMouseWheelFunc(mouseWheel);

    glutMainLoop();
    return 0;
}