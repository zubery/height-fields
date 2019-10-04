/*
  CSCI 480 Computer Graphics
  Assignment 1: Height Fields
  C++ starter code
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <iostream>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

//window width and height
float width = 640.0; 
float height = 480.0; 

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void myinit()
{
  /* setup gl view here */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  //enable depth buffering
  glEnable(GL_DEPTH_TEST); 
}

void display()
{
  /* draw 1x1 cube about origin */
  /* replace this code with your height field implementation */
  /* you may also want to precede it with your 
rotation/translation/scaling */

  //clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  //clear matrix
  glLoadIdentity(); 

  //look at matrix
  gluLookAt(0.0, 0.0, 175.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  //scale rotate and translate to roughly center image at a slight angle
  //includes extra transformations based on mouse/key input
  glScalef(0.5 * g_vLandScale[0], 0.5  * g_vLandScale[1], 
    0.5  * g_vLandScale[2]); 

  glRotatef(-45, 1.0, 0.0, 0.0); 
  glRotatef(-15.0, 0.0, 1.0, 1.0); 
  glRotatef(g_vLandRotate[0], 1.0, 0.0, 0.0); 
  glRotatef(g_vLandRotate[1], 0.0, 1.0, 0.0); 
  glRotatef(g_vLandRotate[2], 0.0, 0.0, 1.0); 

  glTranslatef(-g_pHeightData->nx * 0.5, -g_pHeightData->ny * 0.5, 0);
  glTranslatef(g_vLandTranslate[0] * 100.0, g_vLandTranslate[1] * 100.0, 
    g_vLandTranslate[2] * 100.0); 

  //loops through all the rows of pixels
  for(int i = 0; i < (g_pHeightData->ny) - 1; i++)
  {
    //begin drawing triangle strips
    glBegin(GL_TRIANGLE_STRIP); 

    //loops through all the columns of pixels
    for(int j = 0; j < g_pHeightData->nx; j++)
    {
      unsigned char heightValFirst, heightValSecond; 

      //calculate height values from pic_pixel
      heightValFirst = PIC_PIXEL(g_pHeightData, j, i, 0); 
      heightValSecond = PIC_PIXEL(g_pHeightData, j, i + 1, 0); 

      //draw vertices at x, y, and calculated height value for z
      //color also dependent on height value (varying shades of blue)
      glColor3f(heightValFirst/255.0, heightValFirst/255.0, 1.0);
      glVertex3f(j, i, heightValFirst/10.0); 
      glColor3f(heightValSecond/255.0, heightValSecond/255.0, 1.0); 
      glVertex3f(j, i + 1, heightValSecond/10.0); 
    }

    //end drawing
    glEnd(); 
  }

  //swap buffers
  glutSwapBuffers(); 
}

void reshape(int w, int h)
{
  //set up image size
  glViewport(0, 0, width, height);

  //projection related changes, sets field of view to 60 degrees
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  
  gluPerspective(60.0, width/height, 0.5, 300.0); 

  //modelview related changes
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

void doIdle()
{
  /* do some stuff... */

  /* make the screen update */
  glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandRotate[2] += vMouseDelta[1];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }

  glutPostRedisplay(); 
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }
 
  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      g_ControlState = TRANSLATE;
      break;
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void keyboard(unsigned char key, int x, int y)
{
  //points on 'a'
  if(key == 'a')
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); 
    glutPostRedisplay(); 
  }
  //wireframe on 's'
  else if(key == 's')
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
    glutPostRedisplay(); 
  }
  //filled triangles on 'd'
  else if(key == 'd')
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
    glutPostRedisplay(); 
  }
  //button controls for mac because CTRL doesn't work for scaling
  //q to zoom in, e to zoom out
  else if(key == 'q')
  {
    g_vLandScale[0] += 0.05; 
    g_vLandScale[1] += 0.05; 
    g_vLandScale[2] += 0.05; 
  }
  else if(key == 'e')
  {
    g_vLandScale[0] -= 0.05; 
    g_vLandScale[1] -= 0.05; 
    g_vLandScale[2] -= 0.05; 
  }
  else if(key == 'z')
  {
    g_ControlState = ROTATE;
  }
}

int main (int argc, char ** argv)
{
  if (argc<2)
  {  
    printf ("usage: %s heightfield.jpg\n", argv[0]);
    exit(1);
  }

  g_pHeightData = jpeg_read(argv[1], NULL);
  if (!g_pHeightData)
  {
    printf ("error reading %s.\n", argv[1]);
    exit(1);
  }

  glutInit(&argc,argv);
  
  //creates a window that's double buffered and with depth testing
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(width, height); 
  glutCreateWindow("Ryan's Assignment 1"); 

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  //tells glut to use a particular reshape function
  glutReshapeFunc(reshape); 
  
  /* allow the user to quit using the right mouse button menu */
  g_iMenuId = glutCreateMenu(menufunc);
  glutSetMenu(g_iMenuId);
  glutAddMenuEntry("Quit",0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mousedrag);
  /* callback for idle mouse movement */
  glutPassiveMotionFunc(mouseidle);
  /* callback for mouse button changes */
  glutMouseFunc(mousebutton);
  //callback for keyboard
  glutKeyboardFunc(keyboard);

  /* do initialization */
  myinit();

  glutMainLoop();
  return(0);
}
