/*

Partially completed, it still finds the correct console values ??for me, but it captures the buildings in the wrong place.
To get hydraulic erosion simulation please press left button to start or stop hydraulic erosion simulation.
After hydraulic erosion simulation, you can press the right button several times to place the city in different areas.

*/

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>


using namespace std;

const double PI = 3.14;
const int GSZ = 100;
const int H = 600;
const int W = 600;

double angle = 0;
double numOfFloors = 20;
double colorOfRoof = 1;
double numOfWindows = 2;

double erosion_rate = 0.01;  // Rate of erosion per raindrop
int erosion_active = 0;       // Flag to control erosion process
int city_active = 0;       // Flag to control erosion process

typedef struct {
	int x;
	int z;
} POINT2D;

int cX=0, cZ=0;
bool cityFound = false;

typedef struct {
	double x, y, z;
} POINT3;

POINT3 eye = { 2,24,20 };

double sight_angle = PI;
POINT3 sight_dir = { sin(sight_angle),0,cos(sight_angle) }; // in plane X-Z
double speed = 0;
double angular_speed = 0;

double ground[GSZ][GSZ] = { 0 };
double water[GSZ][GSZ] = { 0 };
bool isVisted[GSZ][GSZ] = { 0 };

double tmp[GSZ][GSZ];
double tmp1[GSZ][GSZ];


void UpdateGround3();
void Smooth();


double calculateDistance(double x1, double y1, double z1, double x2, double y2, double z2) {
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2) + std::pow(z2 - z1, 2));
}


void init()
{
	//                 R     G    B
	glClearColor(0.5, 0.7, 1, 0);// color of window background

	glEnable(GL_DEPTH_TEST);

	int i, j;
	double dist;

	srand(time(0));

	for (i = 0;i < 3000;i++)
		UpdateGround3();

	Smooth();
	for (i = 0;i < 1000;i++)
		UpdateGround3();

}
void Smooth()
{

	for (int i = 1;i < GSZ - 1;i++)
		for (int j = 1;j < GSZ - 1;j++)
		{
			tmp[i][j] = (ground[i - 1][j - 1] + ground[i - 1][j] + ground[i - 1][j + 1] +
				ground[i][j - 1] + ground[i][j] + ground[i][j + 1] +
				ground[i + 1][j - 1] + ground[i + 1][j] + ground[i + 1][j + 1]) / 9.0;
			tmp1[i][j] = (water[i - 1][j - 1] + water[i - 1][j] + water[i - 1][j + 1] +
				water[i][j - 1] + water[i][j] + water[i][j + 1] +
				water[i + 1][j - 1] + water[i + 1][j] + water[i + 1][j + 1]) / 9.0;
		}

	for (int i = 1;i < GSZ - 1;i++)
		for (int j = 1;j < GSZ - 1;j++)
		{
			ground[i][j] = tmp[i][j];
			water[i][j] = tmp1[i][j];
		}
}

void SetColor(double h)
{
	if (h < -0.0)
		glColor3d(0.0, 0.2, 0.4);
	else
	{
		h = fabs(h) / 6;
		// sand
		if (h < 0.02)
			glColor3d(0.8, 0.7, 0.5);
		else	if (h < 0.3)// grass
			glColor3d(0.4 + 0.8 * h, 0.6 - 0.6 * h, 0.2 + 0.2 * h);
		else if (h < 0.9) // stones
			glColor3d(0.4 + 0.1 * h, 0.4 + 0.1 * h, 0.2 + 0.1 * h);
		else // snow
			glColor3d(h, h, 1.1 * h);

	}
}

void UpdateGround3()
{
	double delta = 0.04;
	if (rand() % 2 == 0)
		delta = -delta;
	int x1, y1, x2, y2;
	x1 = rand() % GSZ;
	y1 = rand() % GSZ;
	x2 = rand() % GSZ;
	y2 = rand() % GSZ;
	double a, b;
	if (x1 != x2)
	{
		a = (y2 - y1) / ((double)x2 - x1);
		b = y1 - a * x1;
		for (int i = 0;i < GSZ;i++)
			for (int j = 0;j < GSZ;j++)
			{
				if (i < a * j + b)
				{
					ground[i][j] += delta;
					water[i][j] += delta;

				}

				else
				{
					ground[i][j] -= delta;
					water[i][j] -= delta;

				}
			}
	}

}

void DrawFloor()
{
	int i, j;

	for (i = 1;i < GSZ;i++)
		for (j = 1;j < GSZ;j++)
		{
			glBegin(GL_POLYGON);
			//			glBegin(GL_LINE_LOOP);
			glColor3d(0, 0.2, 0.4);
			glVertex3d(j - GSZ / 2, water[i][j] - 0.1, i - GSZ / 2);
			glVertex3d(j - GSZ / 2, water[i - 1][j] - 0.1, i - 1 - GSZ / 2);
			glVertex3d(j - 1 - GSZ / 2, water[i - 1][j - 1] - 0.1, i - 1 - GSZ / 2);
			glVertex3d(j - 1 - GSZ / 2, water[i][j - 1] - 0.1, i - GSZ / 2);
			glEnd();
		}

	for (i = 1;i < GSZ;i++)
		for (j = 1;j < GSZ;j++)
		{
			//			glBegin(GL_LINE_LOOP);
			glBegin(GL_POLYGON);
			SetColor(ground[i][j]);
			glVertex3d(j - GSZ / 2, ground[i][j], i - GSZ / 2);
			SetColor(ground[i - 1][j]);
			glVertex3d(j - GSZ / 2, ground[i - 1][j], i - 1 - GSZ / 2);
			SetColor(ground[i - 1][j - 1]);
			glVertex3d(j - 1 - GSZ / 2, ground[i - 1][j - 1], i - 1 - GSZ / 2);
			SetColor(ground[i][j - 1]);
			glVertex3d(j - 1 - GSZ / 2, ground[i][j - 1], i - GSZ / 2);
			glEnd();
		}

	// water + transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0, 0.4, 0.7, 0.7);
	glBegin(GL_POLYGON);
	glVertex3d(-GSZ / 2, 0, -GSZ / 2);
	glVertex3d(-GSZ / 2, 0, GSZ / 2);
	glVertex3d(GSZ / 2, 0, GSZ / 2);
	glVertex3d(GSZ / 2, 0, -GSZ / 2);
	glEnd();

	glDisable(GL_BLEND);

}

// n is the number of facets
void DrawCylinder(int n)
{
	double alpha, teta = 2 * PI / n;
	int wigs;

	for (alpha = 0; alpha <= 2 * PI;alpha += teta)
	{
		glBegin(GL_POLYGON);

		// Determine the segment based on the current angle
		wigs = static_cast<int>(alpha / (PI / 2)) % 4;

		// Assign a color to each segment
		switch (wigs)
		{
		case 0:
			glColor3d(0.76, 0.65, 0.46);  // Camel color for the building
			break;
		case 1:
			glColor3d(0.8, 0.7, 0.5);  // Slightly lighter camel color for the building
			break;
		case 2:
			glColor3d(0.76, 0.65, 0.46);  // Camel color for the building
			break;
		case 3:
			glColor3d(0.8, 0.7, 0.5);  // Slightly lighter camel color for the building
			break;
		}

		glVertex3d(sin(alpha), 1, cos(alpha));               // 1
		glVertex3d(sin(alpha + teta), 1, cos(alpha + teta));  // 2
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta));  // 3
		glVertex3d(sin(alpha), 0, cos(alpha));                // 4
		glEnd();
	}
}

void DrawWindows(int n, int numOfWindows)
{
	double alpha, teta = 2 * PI / n;
	int wallsNeeded = numOfWindows * 2 + 1;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta) {
		glBegin(GL_POLYGON);
		// Shadow for inside windows
		glColor3d(0, 0, 0.6); // BLUE color for the inside of the building

		glVertex3d((sin(alpha)) * 0.99, 1, (cos(alpha)) * 0.99);               // 1
		glVertex3d((sin((alpha + teta))) * 0.99, 1, (cos((alpha + teta))) * 0.99); // 2
		glVertex3d((sin((alpha + teta))) * 0.99, 0, (cos((alpha + teta))) * 0.99); // 3
		glVertex3d((sin(alpha)) * 0.99, 0, (cos(alpha)) * 0.99);                 // 4
		glEnd();

		int wigs = static_cast<int>(alpha / (PI / 2)) % 4;

		// Assign a color to each segment
		switch (wigs) {
		case 0:
		case 2:
			glColor3d(0.76, 0.65, 0.46); // Camel color for the building
			break;
		case 1:
		case 3:
			glColor3d(0.8, 0.7, 0.5); // Slightly lighter camel color for the building
			break;
		}

		double startX = sin(alpha);
		double startY = 1;
		double startZ = cos(alpha);
		double endX = sin(alpha + teta);
		double endY = 1;
		double endZ = cos(alpha + teta);

		double distance = calculateDistance(startX, startY, startZ, endX, endY, endZ);

		for (int part = 0; part < wallsNeeded; ++part) {

			glBegin(GL_POLYGON);

			double interpolateFactorStart = static_cast<double>(part) / wallsNeeded;
			double interpolateFactorEnd = static_cast<double>(part + 1) / wallsNeeded;

			double startX_segment = startX + interpolateFactorStart * (endX - startX);
			double startZ_segment = startZ + interpolateFactorStart * (endZ - startZ);

			double endX_segment = startX + interpolateFactorEnd * (endX - startX);
			double endZ_segment = startZ + interpolateFactorEnd * (endZ - startZ);

			if (part % 2 == 0)
			{
				glVertex3d(startX_segment, 1, startZ_segment);               // 1
				glVertex3d(endX_segment, 1, endZ_segment);  // 2
				glVertex3d(endX_segment, 0, endZ_segment);  // 3
				glVertex3d(startX_segment, 0, startZ_segment);                // 4
			}


			glEnd();
		}
	}
}

// n is the number of facets
void DrawRoof(int n, double topr, double bottomr, int colorOfRoof)
{

	double alpha, teta = 2 * PI / n;
	int wigs;

	for (alpha = 0; alpha <= 2 * PI;alpha += teta)
	{
		glBegin(GL_POLYGON);

		wigs = static_cast<int>(alpha / (PI / 2)) % 4;

		switch (colorOfRoof)
		{
		case 0:
			// Assign a color to each segment BLUE
			switch (wigs)
			{
			case 0:
			case 2:
				glColor3d(0.5, 0.5, 0.5);  // Blue color for segment

				break;

			case 1:
			case 3:
				glColor3d(0.6, 0.6, 0.6);  // Slightly lighter blue color

				break;
			}
			break;
		case 1:
			// Assign a color to each segment RED
			switch (wigs)
			{
			case 0:
			case 2:
				glColor3d(1.0, 0.3, 0.3);  // Red color 

				break;

			case 1:
			case 3:
				glColor3d(1.0, 0.4, 0.4);  // Slightly lighter red color 

				break;
			}
			break;
		case 2:
			// Assign a color to each segment BLACK
			switch (wigs)
			{
			case 0:
			case 2:
				glColor3d(0.1, 0.1, 0.1);  // Black color for segment 4 and 6

				break;

			case 1:
			case 3:
				glColor3d(0.25, 0.25, 0.25);  // Slightly lighter black color for segment 5 and 7

				break;
			}
			break;
		}
		// Determine the segment based on the current angle


		glBegin(GL_POLYGON);
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha));// 1
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); //2
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta));// 3
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); //4              // 4
		glEnd();
	}
}

void drawBulding(int numOfFloors, int colorOfRoof, int numOfWindows)
{
	int count = 1;

	for (int i = 1; i <= numOfFloors; i++) {
		glPushMatrix();
		glScaled(6, 3, 3);
		glTranslated(0, count - 1, 0);
		DrawCylinder(4);
		glPopMatrix();


		glPushMatrix();
		glScaled(6, 3, 3);
		glTranslated(0, count, 0);
		DrawWindows(4, numOfWindows);
		glPopMatrix();


		glPushMatrix();
		glScaled(6, 3, 3);
		glTranslated(0, count + 1, 0);
		DrawCylinder(4);
		glPopMatrix();



		if (numOfFloors == i) {

			glPushMatrix();
			glScaled(6, 3, 3);
			glTranslated(0, count + 2, 0);
			DrawRoof(4, 0, 1, colorOfRoof);
			glPopMatrix();
			count++;
		}
		count += 3;

	}
}


void drawCityBuilding()
{
	glPushMatrix();
	glTranslated(cX, ground[cZ][cX], cZ);
	glScaled(0.05, 0.05, 0.05);
	drawBulding(numOfFloors, colorOfRoof, numOfWindows);
	glPopMatrix();

	glPushMatrix();
	glTranslated((cX + 1), ground[cZ][cX + 1], cZ);
	glScaled(0.05, 0.05, 0.05);
	drawBulding(numOfFloors-1, colorOfRoof, numOfWindows);
	glPopMatrix();

	glPushMatrix();
	glTranslated((cX - 1), ground[cZ][cX - 1], cZ);
	glScaled(0.05, 0.05, 0.05);
	drawBulding(numOfFloors+2, colorOfRoof, numOfWindows);
	glPopMatrix();

	glPushMatrix();
	glTranslated(cX, ground[cZ + 1][cX], (cZ + 1));
	glScaled(0.05, 0.05, 0.05);
	drawBulding(numOfFloors - 2, colorOfRoof, numOfWindows);
	glPopMatrix();

	glPushMatrix();
	glTranslated(cX, ground[cZ - 1][cX], (cZ - 1));
	glScaled(0.05, 0.05, 0.05);
	drawBulding(numOfFloors+1, colorOfRoof, numOfWindows);
	glPopMatrix();
}

bool isValid(int x, int z) {
	return (x >= 0 
		&& x < GSZ 
		&& z >= 0 
		&& z < GSZ);
}

bool isNearSea(int x, int z)
{
	return ((isValid(x - 10, z) && ground[z][x - 10] < 0) 
		|| (isValid(x + 10, z) && ground[z][x + 10] < 0 ) 
		|| (isValid(x, z - 10) && ground[z - 10][x] < 0) 
		|| (isValid(x, z + 10) && ground[z + 10][x] < 0));
}

bool isNearRiver(int x , int z )
{
	return ((isValid(x - 10, z) && ground[z][x - 10] < water[z][x - 10]) 
		|| (isValid(x + 10, z) && ground[z][x + 10] < water[z][x + 10]) 
		|| (isValid(x, z - 10) && ground[z - 10][x] < water[z - 10][x]) 
		|| (isValid(x, z + 10) && ground[z + 10][x] < water[z + 10][x]));
}

void FloodFillIterative(int x, int z)
{
	vector <POINT2D> myStack;
	POINT2D current = { x,z };
	myStack.push_back(current);


	while (!myStack.empty())
	{
		// 1. extract last element from stack
		current = myStack.back();
		myStack.pop_back();

		// 2. save current point coordinates
		x = current.x;  
		z = current.z;
		isVisted[z][x] = true;

		// Check if the point meets the criteria for a suitable area
		if (ground[z][x] > 0) 
		{
			// Check if the water level is higher than adjacent terrain
			if (isNearSea(x, z) && isNearRiver(x, z))
			{
				std::cout << "Suitable location found at (" << cX << ", " <<ground[cZ][cX] << ", " << cZ << ")" << std::endl;
				cX = x;
				cZ = z;
				cityFound = true;
				return;
			}
			else
			{
				// Push adjacent points to the stack for further exploration
				if (isValid(x - 10, z) && !isVisted[z][x - 10])
				{
					current.x = x - 10;
					current.z = z;
					myStack.push_back(current);
				}
				if (isValid(x + 10, z) && !isVisted[z][x + 10])
				{
					current.x = x + 10;
					current.z = z;
					myStack.push_back(current);
				}
				if (isValid(x, z - 10) && !isVisted[z - 10][x])
				{
					current.x = x;
					current.z = z - 10;
					myStack.push_back(current);
				}
				if (isValid(x, z + 10) && !isVisted[z + 10][x])
				{
					current.x = x;
					current.z = z + 10;
					myStack.push_back(current);
				}
			}

		}


	}
}



void hydraulicErosion()
{
	if (erosion_active)
	{
		int x = rand() % GSZ;
		int z = rand() % GSZ;
		double erosion_factor = 0.01; // This value can be adjusted based on desired erosion 

		ground[z][x] -= erosion_factor;

		bool found_lower;
		do {
			found_lower = false;
			int lowest_neighbor_x = x;
			int lowest_neighbor_z = z;
			double lowest_height = ground[z][x];

			// Check all neighboring cells
			for (int dx = -1; dx <= 1; dx++) {
				for (int dz = -1; dz <= 1; dz++) {
					int nx = x + dx;
					int nz = z + dz;
					if (isValid(nx,nz)) {
						if (ground[nz][nx] < lowest_height) {
							lowest_height = ground[nz][nx];
							lowest_neighbor_x = nx;
							lowest_neighbor_z = nz;
							found_lower = true;
						}
					}
				}
			}

			if (found_lower) {
				ground[lowest_neighbor_z][lowest_neighbor_x] -= erosion_factor;
				x = lowest_neighbor_x;
				z = lowest_neighbor_z;
			}
		} while (found_lower);
	}
}


// put all the drawings here
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-buffer
	glViewport(0, 0, W, H);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // unity matrix of projection

	glFrustum(-1, 1, -1, 1, 0.75, 300);
	gluLookAt(eye.x, eye.y, eye.z,  // eye position
		eye.x + sight_dir.x, eye.y - 0.3, eye.z + sight_dir.z,  // sight dir
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // unity matrix of model

	//Floor
	DrawFloor(); 

	// HYDRACLIC EROSION 
	hydraulicErosion();

	if (cityFound)
	{
		drawCityBuilding();
	}

 	glutSwapBuffers(); // show all
}

void idle()
{
	int i, j;
	double dist;
	angle += 0.1;

	// ego-motion  or locomotion
	sight_angle += angular_speed;
	// the direction of our sight (forward)
	sight_dir.x = sin(sight_angle);
	sight_dir.z = cos(sight_angle);
	// motion
	eye.x += speed * sight_dir.x;
	eye.y += speed * sight_dir.y;
	eye.z += speed * sight_dir.z;

	glutPostRedisplay();
}

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: // turns the user to the left
		angular_speed += 0.0004;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.0004;
		break;
	case GLUT_KEY_UP: // increases the speed
		speed += 0.005;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.005;
		break;
	case GLUT_KEY_PAGE_UP:
		eye.y += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eye.y -= 0.1;
		break;

	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		erosion_active =! erosion_active;  // Toggle erosion process
	}

	else    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		int startX = rand() % GSZ;
		int startZ = rand() % GSZ;

		if (isValid(startX,startZ))
		{
			FloodFillIterative(startX, startZ);
		}
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Project");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(mouse);

	init();

	glutMainLoop();
}