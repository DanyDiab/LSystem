#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
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
float cameraZoom = 20.0f;

GLuint vao = 0;
GLuint pointsVBO = 0;
GLuint shaderProgram;

void updateCamera() {
    glUseProgram(shaderProgram);
    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,200.0f);
    glm::vec3 targetPos = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 upVec = glm::vec3(0.0f,1.0f,0.0f);

    glm::mat4 view = glm::lookAt(cameraPos,targetPos, upVec);

    float halfWidth = static_cast<float>((windowWidth / 2.0f)) * cameraZoom;
    float halfHeight = static_cast<float>((windowHeight / 2.0f)) * cameraZoom;

    glm::mat4 projection = glm::ortho(-halfWidth,halfWidth,-halfHeight,halfHeight,.1f,100.0f);

    GLint modelLoc = glGetUniformLocation(shaderProgram,"model");
    GLint viewLoc = glGetUniformLocation(shaderProgram,"view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram,"projection");

    glUniformMatrix4fv(modelLoc,1,GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc,1,GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE, glm::value_ptr(projection));
}

std::string readInShader(const std::string& filepath){
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

void generateAndBindVBOVAOs(){
    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);

    GLsizeiptr totalSize = static_cast<GLsizeiptr>(points.size() * sizeof(float));
    glBufferData(GL_ARRAY_BUFFER, totalSize, points.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,0,NULL);
    glEnableVertexAttribArray(0);
}

void init(){
    executeInstructions();

    GLenum err = glewInit();

    compileShaders();
    generateAndBindVBOVAOs();

}

void grabMouseWheel(int32_t wheel, int32_t direction, int32_t x, int32_t y){
    if (direction == 0) {
        return;
    }

    if(direction > 0){
        cameraZoom *= .9f;
    }else{
        cameraZoom *= 1.1f;
    }
    glutPostRedisplay();
}


void update(){
    if (vao == 0) {
        glutSwapBuffers();
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    updateCamera();

    glBindVertexArray(vao);
    int32_t vertexCount = static_cast<int32_t>(points.size() / 2);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glBindVertexArray(0);
    glutSwapBuffers();
    glutPostRedisplay();


}

int main(int argc, char** argv){
    readInJSON();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL");
    init();
    glutDisplayFunc(update);
    glutMouseWheelFunc(grabMouseWheel);
    glutMainLoop();

    return 0;
}
