// Use OpenGL and GLUT to draw a simple cube
// with each face being a different color.  Rotations
// can be applied with the arrow keys, the page up
// key, and the page down key.  The home key will set
// all rotations to 0.

#include "GL/gl.h"
#include "GL/freeglut.h"

int rotateX, rotateY, rotateZ;  // rotation amounts about axes, controlled by keyboard

// ------------- drawing functions for this program -------------------------

void square(float r, float g, float b) {
    glColor3f(r,g,b);         // The color for the square.
    glTranslatef(0,0,0.5);    // Move square 0.5 units forward.
    glNormal3f(0,0,1);        // Normal vector to square (this is actually the default).
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-0.5,-0.5);    // Draw the square (before the
    glVertex2f(0.5,-0.5);     //   the translation is applied)
    glVertex2f(0.5,0.5);      //   on the xy-plane, with its
    glVertex2f(-0.5,0.5);     //   at (0,0,0).
    glEnd();
}

void cube() {

    glPushMatrix();
    square(1,0,0);        // front face is red
    glPopMatrix();

    glPushMatrix();
    glRotatef(180,0,1,0); // rotate square to back face
    square(0,1,1);        // back face is cyan
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90,0,1,0); // rotate square to left face
    square(0,1,0);        // left face is green
    glPopMatrix();

    glPushMatrix();
    glRotatef(90,0,1,0); // rotate square to right face
    square(1,0,1);       // right face is magenta
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90,1,0,0); // rotate square to top face
    square(0,0,1);        // top face is blue
    glPopMatrix();

    glPushMatrix();
    glRotatef(90,1,0,0); // rotate square to bottom face
    square(1,1,0);        // bottom face is yellow
    glPopMatrix();

}

// ----------------------------------------------------------------------

void initGL() {
       // called by main() to do initialization
    rotateX = rotateY = 15;
    rotateZ = 0;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
}

void renderFunction() {
       // called when the display needs to be drawn
       
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);  // Set up the projection.
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-2,2);
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();             // Set up modelview transform. 
    glRotatef(rotateZ,0,0,1);
    glRotatef(rotateY,0,1,0);
    glRotatef(rotateX,1,0,0);
    
    cube();
    
    glutSwapBuffers();
}


void specialKeyFunction(int key, int x, int y) {
        // called when a special key is pressed 
    if ( key == GLUT_KEY_LEFT )
       rotateY -= 15;
    else if ( key == GLUT_KEY_RIGHT )
       rotateY += 15;
    else if ( key == GLUT_KEY_DOWN)
       rotateX += 15;
    else if ( key == GLUT_KEY_UP )
       rotateX -= 15;
    else if ( key == GLUT_KEY_PAGE_UP )
       rotateZ += 15;
    else if ( key == GLUT_KEY_PAGE_DOWN )
       rotateZ -= 15;
    else if ( key == GLUT_KEY_HOME )
       rotateX = rotateY = rotateZ = 0;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("OpenGL - Simple Cube");
    initGL();
    glutDisplayFunc(renderFunction);
    glutSpecialFunc(specialKeyFunction);
    glutMainLoop();
    return 0;
}
