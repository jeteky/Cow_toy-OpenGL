#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <GL/freeglut.h>
#include "FrameXform.h"
#include "WaveFrontOBJ.h"

// 'cameras' stores infomation of 5 cameras.
double cameras[5][9] =
{
	{ 28, 18, 28, 0, 2, 0, 0, 1, 0 },
	{ 28, 18, -28, 0, 2, 0, 0, 1, 0 },
	{ -28, 18, 28, 0, 2, 0, 0, 1, 0 },
	{ -12, 12, 0, 0, 2, 0, 0, 1, 0 },
	{ 0, 100, 0, 0, 0, 0, 1, 0, 0 }
};
int cameraCount = sizeof(cameras) / sizeof(cameras[0]);

int cameraIndex, camID;
vector<FrameXform> wld2cam, cam2wld;
WaveFrontOBJ* cam;

// Variables for 'cow' object.
FrameXform cow2wld;
WaveFrontOBJ* cow;
int cowID;

unsigned floorTexID;

int frame = 0;
int width, height;
int oldX, oldY;

void drawFrame(float len);

//Applied in PA #2//
bool rotation = false;
float dx, dy, distance;
float rot_Angle = 1.0;

int key_Transform = 0;
//----------------//

/*******************************************************************/
//(PA #3) : 과제를 해결하기 위한 변수를 추가적으로 선언하십시오.
/*******************************************************************/

void setCamera()
{
	/*******************************************************************/
	//(PA #3) :카메라를 world 좌표계에 그리는 함수입니다.
	//카메라 시점의 뷰잉 매트릭스를 과제에 활용하십시오.
	/*******************************************************************/
	int i;
	if (frame == 0)
	{
		// intialize camera model.
		cam = new WaveFrontOBJ("camera.obj");	// Read information of camera from camera.obj.
		camID = glGenLists(1);					// Create display list of the camera.
		glNewList(camID, GL_COMPILE);			// Begin compiling the display list using camID.
		cam->Draw();							// Draw the camera. you can do this job again through camID..
		glEndList();							// Terminate compiling the display list.

		// initialize camera frame transforms.
		for (i = 0; i < cameraCount; i++)
		{
			double* c = cameras[i];											// 'c' points the coordinate of i-th camera.
			wld2cam.push_back(FrameXform());								// Insert {0} matrix to wld2cam vector.
			glPushMatrix();													// Push the current matrix of GL into stack.
			glLoadIdentity();												// Set the GL matrix Identity matrix.
			gluLookAt(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8]);		// Setting the coordinate of camera.
			glGetDoublev(GL_MODELVIEW_MATRIX, wld2cam[i].matrix());			// Read the world-to-camera matrix computed by gluLookAt.
			glPopMatrix();													// Transfer the matrix that was pushed the stack to GL.
			cam2wld.push_back(wld2cam[i].inverse());						// Get the camera-to-world matrix.
		}
		cameraIndex = 0;
	}

	// set viewing transformation.
	glLoadMatrixd(wld2cam[cameraIndex].matrix());

	//draw other cameras.
	for (i = 0; i < (int)wld2cam.size(); i++)
	{
		if (i != cameraIndex)
		{
			glPushMatrix();													// Push the current matrix on GL to stack. The matrix is wld2cam[cameraIndex].matrix().
			glMultMatrixd(cam2wld[i].matrix());								// Multiply the matrix to draw i-th camera.

			drawFrame(5);													// Draw x, y, and z axis.
			//float frontColor[] = { 0.2, 0.2, 0.2, 1.0 };
			glEnable(GL_LIGHTING);
			//glMaterialfv(GL_FRONT, GL_AMBIENT, frontColor);				// Set ambient property frontColor.
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, frontColor);				// Set diffuse property frontColor.

			glScaled(0.5, 0.5, 0.5);										// Reduce camera size by 1/2.
			glTranslated(1.1, 1.1, 0.0);									// Translate it (1.1, 1.1, 0.0).
			glCallList(camID);												// Re-draw using display list from camID. 
			glPopMatrix();													// Call the matrix on stack. wld2cam[cameraIndex].matrix() in here.
		}
	}
}

/*********************************************************************************
* Draw x, y, z axis of current frame on screen.
* x, y, and z are corresponded Red, Green, and Blue, resp.
**********************************************************************************/
void drawFrame(float len)
{
	glDisable(GL_LIGHTING);		// Lighting is not needed for drawing axis.
	glBegin(GL_LINES);			// Start drawing lines.
	glColor3d(1, 0, 0);			// color of x-axis is red.
	glVertex3d(0, 0, 0);
	glVertex3d(len, 0, 0);		// Draw line(x-axis) from (0,0,0) to (len, 0, 0). 
	glColor3d(0, 1, 0);			// color of y-axis is green.
	glVertex3d(0, 0, 0);
	glVertex3d(0, len, 0);		// Draw line(y-axis) from (0,0,0) to (0, len, 0).
	glColor3d(0, 0, 1);			// color of z-axis is  blue.
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, len);		// Draw line(z-axis) from (0,0,0) - (0, 0, len).
	glEnd();					// End drawing lines.
}

/*********************************************************************************
* Draw 'cow' object.
**********************************************************************************/
void drawCow()
{
	/*******************************************************************/
	//(PA #3) :오브젝트를 world 좌표계에 그리는 함수입니다.
	//오브젝트의 모델링 매트릭스를 과제에 활용하십시오.
	/*******************************************************************/
	if (frame == 0)
	{
		// Initialization part.

		// Read information from cow.obj.
		cow = new WaveFrontOBJ("cow.obj");

		// Make display list. After this, you can draw cow using 'cowID'.
		cowID = glGenLists(1);				// Create display lists
		glNewList(cowID, GL_COMPILE);		// Begin compiling the display list using cowID
		cow->Draw();						// Draw the cow on display list.
		glEndList();						// Terminate compiling the display list. Now, you can draw cow using 'cowID'.
		glPushMatrix();						// Push the current matrix of GL into stack.
		glLoadIdentity();					// Set the GL matrix Identity matrix.
		glTranslated(0, -cow->bbmin.y, -8);	// Set the location of cow.
		glRotated(-90, 0, 1, 0);			// Set the direction of cow. These information are stored in the matrix of GL.
		glGetDoublev(GL_MODELVIEW_MATRIX, cow2wld.matrix());	// Read the modelview matrix about location and direction set above, and store it in cow2wld matrix.
		glPopMatrix();						// Pop the matrix on stack to GL.
	}

	glPushMatrix();		// Push the current matrix of GL into stack. This is because the matrix of GL will be change while drawing cow.

	// The information about location of cow to be drawn is stored in cow2wld matrix.
	// If you change the value of the cow2wld matrix or the current matrix, cow would rotate or move.
	glMultMatrixd(cow2wld.matrix());

	drawFrame(5);											// Draw x, y, and z axis.
	//float frontColor[] = { 0.8, 0.2, 0.9, 1.0 };
	glEnable(GL_LIGHTING);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, frontColor);		// Set ambient property frontColor.
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, frontColor);		// Set diffuse property frontColor.

	glCallList(cowID);		// Draw cow. 
	glPopMatrix();			// Pop the matrix in stack to GL. Change it the matrix before drawing cow.
}
void cow_rotate() {
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(cow2wld.matrix());
//추가적인코드작성필요
//현재선택된 축에 대하여 오브젝트의 회전 구현
	glGetDoublev(GL_MODELVIEW_MATRIX, cow2wld.matrix());
	glPopMatrix();
	}
/*********************************************************************************
* Draw floor on 3D plane.
**********************************************************************************/
void drawFloor()
{
	if (frame == 0)
	{
		// Initialization part.
		// After making checker-patterned texture, use this repetitively.

		// Insert color into checker[] according to checker pattern.
		const int size = 8;
		unsigned char checker[size * size * 3];
		for (int i = 0; i < size * size; i++)
		{
			if (((i / size) ^ i) & 1)
			{
				checker[3 * i + 0] = 100;
				checker[3 * i + 1] = 100;
				checker[3 * i + 2] = 100;
			}
			else
			{
				checker[3 * i + 0] = 200;
				checker[3 * i + 1] = 200;
				checker[3 * i + 2] = 200;
			}
		}

		// Make texture which is accessible through floorTexID. 
		glGenTextures(1, &floorTexID);
		glBindTexture(GL_TEXTURE_2D, floorTexID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, checker);
	}

	glDisable(GL_LIGHTING);

	// Set background color.

	glColor3d(0.35, 0.2, 0.1);

	// Draw background rectangle.
	glBegin(GL_POLYGON);
	glVertex3f(2000, -0.2, 2000);
	glVertex3f(2000, -0.2, -2000);
	glVertex3f(-2000, -0.2, -2000);
	glVertex3f(-2000, -0.2, 2000);
	glEnd();


	// Set color of the floor.
	// Assign checker-patterned texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, floorTexID);

	// Draw the floor. Match the texture's coordinates and the floor's coordinates resp. 
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(-12, -0.1, -12);		// Texture's (0,0) is bound to (-12,-0.1,-12).
	glTexCoord2d(1, 0);
	glVertex3d(12, -0.1, -12);		// Texture's (1,0) is bound to (12,-0.1,-12).
	glTexCoord2d(1, 1);
	glVertex3d(12, -0.1, 12);		// Texture's (1,1) is bound to (12,-0.1,12).
	glTexCoord2d(0, 1);
	glVertex3d(-12, -0.1, 12);		// Texture's (0,1) is bound to (-12,-0.1,12).
	glEnd();

	glDisable(GL_TEXTURE_2D);
	drawFrame(5);				// Draw x, y, and z axis.
}


/*********************************************************************************
* Call this part whenever display events are needed.
* Display events are called in case of re-rendering by OS. ex) screen movement, screen maximization, etc.
* Or, user can occur the events by using glutPostRedisplay() function directly.
* this part is called in main() function by registering on glutDisplayFunc(display).
**********************************************************************************/
void display()
{
	glClearColor(0, 0.6, 0.8, 1);									// Clear color setting

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear the screen
	setCamera();													// Locate the camera's position, and draw all of them.

	drawFloor();													// Draw floor.
	drawCow();														// Draw cow.

	glutSwapBuffers();
	frame += 1;
}

/*********************************************************************************
* Call this part whenever size of the window is changed.
* This part is called in main() function by registering on glutReshapeFunc(reshape).
**********************************************************************************/
void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);            // Select The Projection Matrix
	glLoadIdentity();                       // Reset The Projection Matrix
	// Define perspective projection frustum
	double aspect = width / double(height);
	gluPerspective(45, aspect, 1, 1024);
	glMatrixMode(GL_MODELVIEW);             // Select The Modelview Matrix
	glLoadIdentity();                       // Reset The Projection Matrix
}

//------------------------------------------------------------------------------
void initialize()
{
	// Set up OpenGL state
	glShadeModel(GL_FLAT);			 // Set Flat Shading
	glEnable(GL_DEPTH_TEST);         // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);          // The Type Of Depth Test To Do
	// Use perspective correct interpolation if available
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	// Initialize the matrix stacks
	reshape(width, height);
	// Define lighting for the scene
	float light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
	float light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	float light_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glEnable(GL_LIGHT0);
}

/*********************************************************************************
* Call this part whenever mouse button is clicked.
* This part is called in main() function by registering on glutMouseFunc(onMouseButton).
**********************************************************************************/
void onMouseButton(int button, int state, int x, int y)
{
	y = height - y - 1;
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			printf("Left mouse click at (%d, %d)\n", x, y);

			// Save current clicked location of mouse here, and then use this on onMouseDrag function. 
			oldX = x;
			oldY = y;
		}

		//Applied in PA #2 //
		else {
			distance = 0.0;
		}
		//-----------------//

	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		printf("Right mouse click at (%d, %d)\n", x, y);
	}
	glutPostRedisplay();
}


/*********************************************************************************
* Call this part whenever user drags mouse.
* Input parameters x, y are coordinate of mouse on dragging.
* Value of global variables oldX, oldY is stored on onMouseButton,
* Then, those are used to verify value of x - oldX,  y - oldY to know its movement.
**********************************************************************************/
void onMouseDrag(int x, int y)
{
	y = height - y - 1;
	printf("in drag (%d, %d)\n", x - oldX, y - oldY);

	//Applied in PA #2//
	dx = x - oldX;
	dy = y - oldY;
	distance = sqrt(pow(dx,2) + pow(dy,2));

	oldX = x;
	oldY = y;
	//----------------//

	glutPostRedisplay();
}

/*********************************************************************************
* Call this part whenever user types keyboard.
* This part is called in main() function by registering on glutKeyboardFunc(onKeyPress).
**********************************************************************************/
void onKeyPress(unsigned char key, int x, int y)
{
	// If a number is pressed, alter the camera corresponding the number.
	if ((key >= '0') && (key <= '5'))
		cameraIndex = key - '0';

	if (cameraIndex >= (int)wld2cam.size())
		cameraIndex = 0;
	
	//Applied in PA #2 //
	if ((key == 'r')) {
		rotation = !rotation;
	}

	if ((key == 'x')) //빨강
	{
		key_Transform = 0; // X축(빨강)정면에서 고정인상태로 반시계방향회전
	}
	if ((key == 'y')) //그린
	{
		key_Transform = 1; //y축(그린)정면에서 고정인상태로 반시계방향회전
	}
	if ((key == 'z')) //블루
	{
		key_Transform = 2; //z축(블루)이 정면에서 고정인상태로 반시계방향회전
	}
	//-----------------//

	/*******************************************************************/
	//(PA #3) :추가적인 코드 작성이 필요합니다.
	//모델링 공간과 뷰잉 공간을 전환할 수 있도록 키보드 입력을 적용 하십시오
	/*******************************************************************/

	glutPostRedisplay();
}

void idle() {
	glPushMatrix();

	// Applied in PA #2 //
	glLoadIdentity();
	glMultMatrixd(cow2wld.matrix());
	if (rotation == true) {
		if (key_Transform == 0) //빨강
			glRotatef(rot_Angle, 1.0, 0, 0);
		else if (key_Transform == 1) //그린
			glRotatef(rot_Angle, 0, 1.0, 0);
		else if (key_Transform == 2) //블루
			glRotatef(rot_Angle, 0, 0, 1.0);
	}
	if (rotation == false) {
		if (key_Transform == 0) //빨강
		{
			glTranslated(0.05 * distance, 0, 0);
		}
		else if (key_Transform == 1) //그린
		{
			glTranslated(0, 0.05 * distance, 0);
		}
		else if (key_Transform == 2) //블루
		{
			glTranslated(0, 0, 0.05 * distance);
		}
	}
	glGetDoublev(GL_MODELVIEW_MATRIX, cow2wld.matrix());
	//-----------------//

	/*******************************************************************/
	//(PA #3) :추가적인 코드 작성 및 수정이 필요합니다.
	//뷰잉 공간에서의 오브젝트의 Transformation을 적용하십시오
	/*******************************************************************/

	glPopMatrix();
	glutPostRedisplay();
}

//------------------------------------------------------------------------------
void main(int argc, char* argv[])
{
	width = 800;
	height = 600;
	frame = 0;
	glutInit(&argc, argv);							// Initialize openGL.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	// Initialize display mode. This project will use double buffer and RGB color.
	glutInitWindowSize(width, height);				// Initialize window size.
	glutInitWindowPosition(100, 100);				// Initialize window coordinate.
	glutCreateWindow("PA3");
	glutDisplayFunc(display);						// Register display function to call that when drawing screen event is needed.
	glutReshapeFunc(reshape);						// Register reshape function to call that when size of the window is changed.
	glutKeyboardFunc(onKeyPress);					// Register onKeyPress function to call that when user presses the keyboard.
	glutMouseFunc(onMouseButton);					// Register onMouseButton function to call that when user moves mouse.
	glutMotionFunc(onMouseDrag);					// Register onMouseDrag function to call that when user drags mouse.

	// Applied in PA #2 //
	glutIdleFunc(idle);

	initialize();									// Initialize the other thing.
	glutMainLoop();									// Execute the loop which handles events.
}
