#include <iostream>
#include "./headers/freeglut.h"
using namespace std;

void update(){
    glutSwapBuffers();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL");
    glutDisplayFunc(update);

    glutMainLoop();
    return 0;
}