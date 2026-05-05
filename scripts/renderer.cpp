#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <numbers>
#include <cmath>
#include <stack>
#include <nlohmann/json.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "./headers/tokens.hpp"
#include "./headers/turtle.hpp"

using namespace std;
using json = nlohmann::json;

int windowWidth = 1000;
int windowHeight = 1000;
float cameraZoom = 45.0f; 

GLuint vao = 0;
GLuint pointsVBO = 0;
GLuint shaderProgram;

glm::vec3 cameraPos = glm::vec3(0.0f, 100.0f, 300.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = -15.0f;
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;
bool isWarping = false;

float deltaTime = 0.0f;
int lastFrame = 0;

bool keys[256] = {false};


std::vector<float> cylinderPoints;


std::vector<float>generateCylinderVertices(int numPoints){
    auto generateCirclePoints = [](int numPoints){

        std::vector<float> circlePoints;

        float stepSize = (2 * M_PI) / numPoints;

        for(int i = 0; i < numPoints; i++){
            float x = cos(i * stepSize);
            float y = sin(i * stepSize);

            circlePoints.push_back(x);
            circlePoints.push_back(y);
        }

        return circlePoints;
    };

    std::vector<float> circle1 = generateCirclePoints(numPoints / 2);
    std::vector<float> circle2 = generateCirclePoints(numPoints / 2);

    std::vector<float> cylinderPoints;
    int p1 = 0;
    int p2 = 0;
    for(int i = 0; i < numPoints / 4; i++){
        cylinderPoints.push_back(circle1.at(p1));
        cylinderPoints.push_back(circle1.at(p1 + 1));

        cylinderPoints.push_back(circle1.at(p2));
        cylinderPoints.push_back(circle1.at(p2 + 1));

        p1 += 2;
        p2 += 2;
    }


    return {1.0};
}
void updateCamera() {
    glUseProgram(shaderProgram);
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    glm::mat4 projection = glm::perspective(glm::radians(cameraZoom), aspectRatio, 0.1f, 10000.0f);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

std::string readInShader(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void compileShaders() {
    std::string vertexCode = readInShader("./shaders/vert.glsl");
    std::string fragmentCode = readInShader("./shaders/frag.glsl");

    if (vertexCode.empty() || fragmentCode.empty()) {
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void generateAndBindVBOVAOs() {
    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);

    GLsizeiptr totalSize = static_cast<GLsizeiptr>(points.size() * sizeof(float));
    glBufferData(GL_ARRAY_BUFFER, totalSize, points.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
}

void init() {
    cylinderPoints = generateCylinderVertices(20);
    executeInstructions();

    GLenum err = glewInit();

    compileShaders();
    generateAndBindVBOVAOs();

    glutSetCursor(GLUT_CURSOR_NONE);
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void grabMouseWheel(int32_t wheel, int32_t direction, int32_t x, int32_t y) {
    if (direction == 0) {
        return;
    }

    if (direction > 0) {
        cameraZoom -= 2.0f;
    } else {
        cameraZoom += 2.0f;
    }

    if (cameraZoom < 1.0f) {
        cameraZoom = 1.0f;
    }
    if (cameraZoom > 120.0f) {
        cameraZoom = 120.0f;
    }
}

void keyboardDown(unsigned char key, int x, int y) {
    keys[key] = true;
    if (key == 27) {
        exit(0);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouseMove(int x, int y) {
    if (isWarping) {
        isWarping = false;
        return;
    }

    if (firstMouse) {
        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);
        firstMouse = false;
    }

    float xOffset = static_cast<float>(x) - lastX;
    float yOffset = lastY - static_cast<float>(y);

    lastX = static_cast<float>(x);
    lastY = static_cast<float>(y);

    float sensitivity = 0.15f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    if (x != centerX || y != centerY) {
        isWarping = true;
        glutWarpPointer(centerX, centerY);
        lastX = static_cast<float>(centerX);
        lastY = static_cast<float>(centerY);
    }
}

void processInput() {
    float cameraSpeed = 150.0f * deltaTime;
    if (keys['w'] || keys['W']) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (keys['s'] || keys['S']) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (keys['a'] || keys['A']) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (keys['d'] || keys['D']) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (keys['q'] || keys['Q']) {
        cameraPos -= cameraUp * cameraSpeed;
    }
    if (keys['e'] || keys['E']) {
        cameraPos += cameraUp * cameraSpeed;
    }
}

void update() {
    int currentFrame = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    processInput();

    if (vao == 0) {
        glutSwapBuffers();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateCamera();

    glBindVertexArray(vao);
    int32_t vertexCount = static_cast<int32_t>(points.size() / 3);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    readInJSON();
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("L-System 3D");

    glEnable(GL_DEPTH_TEST);

    init();

    glutDisplayFunc(update);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMove);
    glutMouseWheelFunc(grabMouseWheel);
    
    glutIdleFunc(update);

    glutMainLoop();

    return 0;
}