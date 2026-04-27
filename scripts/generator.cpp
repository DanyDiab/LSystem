#include <iostream>
#include "./headers/freeglut.h"
using namespace std;

void draw()
{
    // code for rendering here
    glutSwapBuffers();   // swapping image buffer for double buffering
    glutPostRedisplay(); // redrawing. Omit this line if you don't want constant redraw
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // enabling double buffering and RGBA
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL"); // creating the window
    glutFullScreen();           // making the window full screen
    glutDisplayFunc(draw);      // draw is your function for redrawing the screen

    glutMainLoop();

    return 0;
}