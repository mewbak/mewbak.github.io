
// A framework for writing simple applications with GLUT, with
// support for animation and for mouse and keyboard events.
// Note that you need to uncomment some lines in main() to
// enable animation or mouse events.  The program is set up
// to apply rotations to the entire scene.  The rotation is
// controlled by the arrow keys.  No actual drawing is done.

#include "GL/gl.h"
#include "GL/freeglut.h"
#include "stdio.h"

// --------------------------------- global variables --------------------------------

int frameNumber = 0;     // For use in animation.

float rotateX = 0;   // rotations for a simple viewing transform, applied to the
float rotateY = 0;   //    the entire scene, controlled by the arrow and HOME keys.

float xmin = -1;  // limits of visible coordinate values, for use in setting up projection
float xmax = 1;   //    (actual limits can be adjusted to preserve aspect ration)
float ymin = -1;
float ymax = 1;
float zmin = -2;
float zmax = 2;

// ------------------------ OpenGL initialization and rendering -----------------------

void initGL() {
      // called by main() to initialize the OpenGL drawing context
      
    glClearColor(0.0, 0.0, 0.0, 1.0); // background color

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

}  // end initGL()


void renderFunction() {
        // called whenever the display needs to be redrawn

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glLoadIdentity();             // Set with no modeling transform. 
    
    glRotatef(rotateX, 1, 0, 0);  // Simple viewing transform, controlled by arrow keys
    glRotatef(rotateY, 0, 1, 0);
    
    // TODO: INSERT DRAWING CODE HERE
    
    glutSwapBuffers();  // (required for double-buffered drawing)
}

void reshapeFunction(int width, int height) {
        // called when the window changes size, and when it is first opened
    glViewport(0,0,width,height);  // Must be called!
    
    glMatrixMode(GL_PROJECTION);   // Set up a simple orthographic projection, preserving aspect ratio
    glLoadIdentity();              //    TODO: really should change the projection.
    glOrtho(xmin, xmax, ymin, ymax, zmin, zmax);
    glMatrixMode(GL_MODELVIEW);
    printf("Reshaped to width %d, height %d\n", width, height);
}


// --------------- support for animation ------------------------------------------

int animating = 0;      // 0 or 1 to indicate whether an animation is in progress;
                        // do not change directly; call startAnimation() and pauseAnimation()

void updateFrame() {
      // this is called before each frame of the animation.
   // TODO: INSERT CODE TO UPDATE DATA USED IN DRAWING A FRAME
   frameNumber++;
   printf("frame number %d\n", frameNumber);
}

void timerFunction(int timerID) {
      // used for animation; do not call this directly
    if (animating) {
        updateFrame();
        glutTimerFunc(30, timerFunction, 0);
        glutPostRedisplay();
    }
}

void startAnimation() {
      // call this to start or restart the animation
   if ( ! animating ) {
       animating = 1;
       glutTimerFunc(30, timerFunction, 0);
   }
}

void pauseAnimation() {
       // call this to pause the animation
    animating = 0;
}


// --------------- keyboard event functions ---------------------------------------

void keyFunction(unsigned char ch, int x, int y) {
        // called when the user types an ASCII character
    // TODO: INSERT CHARACTER-HANDLING CODE
    glutPostRedisplay();
    printf("User typed %c, mouse at (%d,%d)\n", ch, x, y);
}

void specialKeyFunction(int key, int x, int y) {
        // called when the user presses a special key, such as an arrow key
    // TODO: INSERT/MODIFY KEY-HANDLING CODE
    if ( key == GLUT_KEY_LEFT )
       rotateY -= 15;
    else if ( key == GLUT_KEY_RIGHT )
       rotateY += 15;
    else if ( key == GLUT_KEY_DOWN)
       rotateX += 15;
    else if ( key == GLUT_KEY_UP )
       rotateX -= 15;
    else if ( key == GLUT_KEY_HOME )
       rotateX = rotateY = 0;
    glutPostRedisplay();
    printf("User pressed special key with code %d; mouse at (%d,%d)\n", key, x, y);
}


// --------------- mouse event functions and support for dragging -----------------

int dragging;        // 0 or 1 to indicate whether a drag operation is in progress
int dragButton;      // which button started the drag operation
int startX, startY;  // mouse position at start of drag
int prevX, prevY;    // previous mouse position during drag

void mouseFunction(int button, int buttonState, int x, int y) {
       // called to respond to mouse press and mouse release events
   if (buttonState == GLUT_DOWN) {  // a mouse button was pressed
       if (dragging)
          return;  // ignore a second button press during a draw operation
       // TODO:  INSERT CODE TO RESPOND TO MOUSE PRESS
       dragging = 1;  // Might not want to do this in all cases
       dragButton = button;
       startX = prevX = x;
       startY = prevY = y;
       printf("Mouse button %d down at (%d,%d)\n", button, x, y);
   }
   else {  // a mouse button was released
       if ( ! dragging || button != dragButton )
           return; // this mouse release does not end a drag operation
       dragging = 0;
       // TODO:  INSERT CODE TO CLEAN UP AFTER THE DRAG (generally not needed)
       printf("Mouse button %d up at (%d,%d)\n", button, x, y);
   }
}

void mouseDragFunction(int x, int y) {
        // called to respond when the mouse moves during a drag
    if ( ! dragging )
        return;  // this is not part of a drag that we want to respond to
    // TODO:  INSERT CODE TO RESPOND TO NEW MOUSE POSITION
    prevX = x;
    prevY = y;
    printf("Mouse dragged to (%d,%d)\n", x, y);
}


// ----------------- main routine -------------------------------------------------

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500,500);        // size of display area, in pixels
    glutInitWindowPosition(100,100);    // location in window coordinates
    glutCreateWindow("OpenGL Program"); // parameter is window title  
    glutDisplayFunc(renderFunction);    // called when the window needs to be redrawn
    glutReshapeFunc(reshapeFunction);   // called when the size of the window changes

    //Uncomment one or both of the next two lines for keyboard event handling
    //glutKeyboardFunc(keyFunction);      // called when user types a character
    //glutSpecialFunc(specialKeyFunction);// called when user presses a special key

    //Uncomment the next line to handle mouse presses; the next two for mouse dragging
    //glutMouseFunc(mouseFunction);       // called for mousedown and mouseup events
    //glutMotionFunc(mouseDragFunction);  // called when mouse moves, during a drag gesture

    //Uncomment the next line to start a timer-controlled animation
    //startAnimation();

    glutMainLoop(); // Run the event loop!  This function does not return.
    return 0;
}
