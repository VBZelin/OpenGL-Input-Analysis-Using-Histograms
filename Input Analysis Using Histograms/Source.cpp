//Zelin edited
//Time: 12:30pm
//Date: 10/12/2015

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cfloat>
#include <cmath>
#include <gl/glew.h>
#include <GL/glut.h>
#include <GL/GL.h> 
#include <time.h>
#pragma comment(lib, "glew32.lib")

using namespace std;

// File names
string currentFile = "normal.dat";

// Histogram
int numIntervals = 30;
string numIntervalsText;
float mean = 2.2, deviation = 1, beta = 1.25;
float pi = 3.1415;
float bins;
float* endPoints;
float* endPointsInAxies;
float* prob;

// string content
string instructionContent = "The parameters can be changed in this application by using Up, Down, Left and Right arrow keys.";
string welcomeContent = "WELCOME TO ZELIN'S PROGRAM";

// Input data
float* stats;
int numDataPoints;
float minimum, maximum, maxProb, finalMaxProb, maxPdfProb, maxBetaProb;
string maximumText, minimumText, maxProbText, meanText = "2.2", deviationText = "1", betaText = "1.25", parameterStepText = "0.02";
float pdfParameterStepSize = 0.02;
float expoParameterStepSize = 0.02;

// Function logic
bool pdf = true, expo = false;

// Drawing parameters
float width = 800.0, height = 600.0;
float textX, textY;

// int anglePlus
int anglePlus = 1;

// mouse function
int mouseX;
int mouseY;
float worldW, worldH;
float mouseXinWorld;
float mouseYinWorld;
string mouseYtext;
string mouseXtext;

// refresh frame
static void Timer(int value) {
	// set parameter
	anglePlus += 0.1;
	// redisplay
	glutPostRedisplay();
	// time last
	glutTimerFunc(1000, Timer, 0);
}

// system time menu
const std::string currentDateTime() {
	// set current time
	time_t     now = time(0);
	// time struct
	struct tm  tstruct;
	// set buffer
	char       buf[80];
	// get local time 
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

// draw text function
void drawText(string text, float x, float y) {
	// set text position
	glRasterPos2f(x, y);
	// display each letter by using bitmap
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
	}
}

// print text
void printText(string text, int x, int y)
{
	// set text position
	glWindowPos2f(x, y);
	// display each letter by using bitmap
	for (int i = 0; i < text.length(); i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text.data()[i]);
	}
}

// user's operation
void keyboard(int key, int x, int y)
{
	// user's input checking
	switch (key) {
		// up key
	case GLUT_KEY_UP:
		// check if it is pdf function
		if (pdf) {
			if (deviation < 3) {
				deviation += pdfParameterStepSize;
				if (deviation >= 3) {
					deviation = 3;
				}
				ostringstream convert;   // stream used for the conversion
				convert << deviation;      // insert the textual representation of 'Number' in the characters in the stream
				deviationText = convert.str();
				convert.str("");
				convert.clear();
			}
		}
		// check if it is expo function
		if (expo) {
			if (beta < 6) {
				beta += expoParameterStepSize;
				if (beta >= 6) {
					beta = 6;
				}
				ostringstream convert;   // stream used for the conversion
				convert << beta;      // insert the textual representation of 'Number' in the characters in the stream
				betaText = convert.str();
				convert.str("");
				convert.clear();
			}
		}
		break;
		// down key
	case GLUT_KEY_DOWN:
		if (pdf) {
			if (deviation > 0.02) {
				deviation -= pdfParameterStepSize;
				if (deviation < 0.02) {
					deviation = 0.02;
				}
				ostringstream convert;   // stream used for the conversion
				convert << deviation;      // insert the textual representation of 'Number' in the characters in the stream
				deviationText = convert.str();
				convert.str("");
				convert.clear();
			}
		}
		if (expo) {
			if (beta > 0.1) {
				beta -= expoParameterStepSize;
				if (beta <= 0.1) {
					beta = 0.1;
				}
				ostringstream convert;   // stream used for the conversion
				convert << beta;      // insert the textual representation of 'Number' in the characters in the stream
				betaText = convert.str();
				convert.str("");
				convert.clear();
			}
		}
		break;
		// left key
	case GLUT_KEY_LEFT:
		// we need to limit mean range, mean >= 0
		if (mean >= 0) {
			mean -= pdfParameterStepSize;
			if (mean <= 0) {
				mean = 0;
			}
			ostringstream convert;   // stream used for the conversion
			convert << mean;      // insert the textual representation of 'Number' in the characters in the stream
			meanText = convert.str();
			convert.str("");
			convert.clear();
		}
		break;
		// right key
	case GLUT_KEY_RIGHT:
		// we need to limit mean range, mean < 5
		if (mean < 5) {
			mean += pdfParameterStepSize;
			if (mean >= 5) {
				mean = 5;
			}
			ostringstream convert;   // stream used for the conversion
			convert << mean;      // insert the textual representation of 'Number' in the characters in the stream
			meanText = convert.str();
			convert.str("");
			convert.clear();
		}
		break;
	}
	// redisplay
	glutPostRedisplay();
}

// read file function
void readFile(string fileName)
{
	ifstream inFile(fileName);
	// check if file is open
	if (!inFile.is_open())
	{
		cout << fileName << " couldn't be opened.\n";
		system("pause");
		exit(1);
	}
	// read numDataPoints from file
	inFile >> numDataPoints;
	// Memory management.
	if (stats != NULL)
		delete stats;
	stats = new float[numDataPoints];
	minimum = FLT_MAX;
	maximum = -FLT_MAX;
	// Read the data and compute minimum and maximum.
	for (int i = 0; i < numDataPoints; i++)
	{
		inFile >> stats[i];
		if (stats[i] < minimum)
			minimum = stats[i];
		if (stats[i] > maximum)
			maximum = stats[i];
	}

	// convert maximum value into string
	ostringstream convert;   // stream used for the conversion
	convert << maximum;      // insert the textual representation of 'Number' in the characters in the stream
	maximumText = convert.str();
	convert.str("");
	convert.clear();
	// convert minimum value into string
	convert << minimum;
	minimumText = convert.str();
	convert.str("");
	convert.clear();
	// convert numIntervals value into string
	convert << numIntervals;
	numIntervalsText = convert.str();
	convert.str("");
	convert.clear();
}

// compute probability
void computeProbability(int numIntervals)
{
	// Delete previously allocated memory
	if (prob != NULL)
		delete prob;
	if (endPoints != NULL)
		delete endPoints;
	if (endPointsInAxies != NULL)
		delete endPointsInAxies;
	// calculate bins
	bins = (maximum - minimum) / numIntervals;
	// set array
	endPoints = new float[numIntervals];
	endPointsInAxies = new float[numIntervals];
	prob = new float[numIntervals];
	// calculate endPoints
	for (int i = 0; i < numIntervals; i++) {
		endPoints[i] = minimum + bins * (i + 1);
		endPointsInAxies[i] = 0.1 + 1 / (float)numIntervals * (i + 1);
	}
	// get maxProb
	for (int x = 0; x < numIntervals; x++) {
		int n = 0;
		for (int i = 0; i < numDataPoints; i++) {
			if (x == 0) {
				if (stats[i] <= endPoints[x]) {
					n++;
				}
			}
			else {
				if (stats[i] > endPoints[x - 1] && stats[i] <= endPoints[x]) {
					n++;
				}
			}
		}
		// calculate current prob
		prob[x] = (float)n / (float)numDataPoints;
		if (prob[x] > maxProb)
			maxProb = prob[x];
	}
	// convert maxProb into string
	finalMaxProb = maxProb / ((maximum - minimum) / numIntervals);
	ostringstream convert;
	convert << finalMaxProb;
	maxProbText = convert.str();
	convert.str("");
	convert.clear();
	// reset maxProb, very important!!!
	maxProb = 0;
}

void display(void)
{
	readFile(currentFile);
	computeProbability(numIntervals);
	/* clear all pixels  */
	glClear(GL_COLOR_BUFFER_BIT);

	// Reset Modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);

	// main text display
	textX = (width - 100) / width + 0.1;
	textY = (height - 50) / height;

	glLineWidth(1);
	glColor3f(1.0, 1.0, 1.0);

	/*draw x and y axes*/
	glBegin(GL_LINES);
	glVertex2f(0, 0);
	glVertex2f(1 * width / height, 0);
	glVertex2f(0, 0);
	glVertex2f(0, 0.6);
	glEnd();

	// draw peak line
	glBegin(GL_LINES);
	glVertex2f(0, 0.5);
	glVertex2f(0.02, 0.5);
	glEnd();

	// draw histogram
	glColor3f(0, 1.0, 0);
	glBegin(GL_LINES);
	glVertex2f(0.1, 0);
	glVertex2f(endPointsInAxies[29], 0);
	glEnd();
	for (int i = 0; i < numIntervals; i++) {
		if (i == 0) {
			glBegin(GL_LINE_STRIP);
			glVertex2f(0.1, 0);
			glVertex2f(0.1, prob[i] / ((maximum - minimum) / numIntervals) / finalMaxProb / 2);
			glVertex2f(endPointsInAxies[i], prob[i] / ((maximum - minimum) / numIntervals) / finalMaxProb / 2);
			glVertex2f(endPointsInAxies[i], 0);
			glEnd();
		}
		else {
			glBegin(GL_LINE_STRIP);
			glVertex2f(endPointsInAxies[i - 1], 0);
			glVertex2f(endPointsInAxies[i - 1], prob[i] / ((maximum - minimum) / numIntervals) / finalMaxProb / 2);
			glVertex2f(endPointsInAxies[i], prob[i] / ((maximum - minimum) / numIntervals) / finalMaxProb / 2);
			glVertex2f(endPointsInAxies[i], 0);
			glEnd();
		}
	}

	// draw Input Analysis 
	glColor3f(1.0, 0, 0);
	if (pdf) {
		maxPdfProb = exp(-pow(minimum - mean, 2) / (2 * pow(deviation, 2))) / (deviation*sqrt(2 * pi));
		for (float x = minimum; x < maximum; x += 0.01) {
			float y = exp(-pow(x - mean, 2) / (2 * pow(deviation, 2))) / (deviation*sqrt(2 * pi));
			float yNext = exp(-pow(x + 0.01 - mean, 2) / (2 * pow(deviation, 2))) / (deviation*sqrt(2 * pi));
			if (yNext > maxPdfProb) {
				maxPdfProb = yNext;
			}
		}
		for (float x = minimum; x < maximum; x += 0.01) {
			float y = exp(-pow(x - mean, 2) / (2 * pow(deviation, 2))) / (deviation*sqrt(2 * pi));
			float yNext = exp(-pow(x + 0.01 - mean, 2) / (2 * pow(deviation, 2))) / (deviation*sqrt(2 * pi));
			glBegin(GL_LINE_STRIP);
			glVertex2f(0.1 + (x - minimum) / (maximum - minimum), y / maxPdfProb / 2);
			glVertex2f(0.1 + (x + 0.01 - minimum) / (maximum - minimum), yNext / maxPdfProb / 2);
			glEnd();
		}
	}
	if (expo) {
		maxBetaProb = exp(-0 / beta) / beta;
		for (float x = minimum; x < maximum; x += 0.01) {
			float y = exp(-x / beta) / beta;
			float yNext = exp(-(x + 0.01) / beta) / beta;
			if (yNext > maxBetaProb) {
				maxBetaProb = yNext;
			}
		}
		for (float x = minimum; x < maximum; x += 0.01) {
			float y = exp(-x / beta) / beta;
			float yNext = exp(-(x + 0.01) / beta) / beta;
			glBegin(GL_LINE_STRIP);
			glVertex2f(0.1 + (maximum - minimum) / numIntervals / 2 / (maximum - minimum) + (x - minimum) / (maximum - minimum), y / maxBetaProb / 2);
			glVertex2f(0.1 + (maximum - minimum) / numIntervals / 2 / (maximum - minimum) + (x + 0.01 - minimum) / (maximum - minimum), yNext / maxBetaProb / 2);
			glEnd();
		}
	}

	// draw axies text
	glColor3f(0, 1.0, 0);
	drawText("Data", 1 * width / height - 0.02, 0.01);
	drawText("Probability Density", 0.01, 0.6);
	drawText("Peak: " + maxProbText, 0.01, 0.53);

	// draw main screen text in right side
	glColor3f(0, 1.0, 0);
	// draw file
	drawText("File: " + currentFile, textX, 0.6);
	// draw minimum
	drawText("Min: " + minimumText, textX, 0.57);
	// draw maximum
	drawText("Max: " + maximumText, textX, 0.54);
	// draw number of intervals
	drawText("Num of Intervals: " + numIntervalsText, textX, 0.51);

	glColor3f(1.0, 0, 0);
	if (pdf) {
		// draw mean text
		drawText("Current Mean: " + meanText, textX, 0.48);
		// draw deviation text
		drawText("Current Deviation: " + deviationText, textX, 0.45);
		// draw parameter text
		drawText("Parameter Step: " + parameterStepText, textX, 0.42);
	}
	if (expo) {
		// draw beta text
		drawText("Current Beta: " + betaText, textX, 0.48);
		// draw parameter text
		drawText("Parameter Step: " + parameterStepText, textX, 0.45);
	}

	// draw main screen text in left side
	glColor3f(1.0, 1.0, 0);
	// draw title
	drawText(welcomeContent, 0.01, textY);
	// draw name
	drawText("NAME: Zelin", 0.01, textY - 0.06);
	// draw UIN
	drawText("UIN: 00000000", 0.01, textY - 0.09);
	// draw date
	drawText("SYSTEM TIME: " + currentDateTime(), 0.01, textY - 0.12);
	// draw operation instruction
	drawText("INSTRUCTION: " + instructionContent, 0.01, textY - 0.15);
	// draw other things
	drawText("Also, you can select coordinates by using mouse cursor on the figure!", 0.01, textY - 0.18);
	drawText("PLEASE ENJOY!!!", 0.01, textY - 0.24);

	drawText("Current Probility Density: " + mouseYtext, textX, 0.39);
	drawText("Current X-Axies: " + mouseXtext, textX, 0.36);

	/* don't wait!
	* start processing buffered OpenGL routines
	*/
	glFlush();
	glutSwapBuffers();
}

static void reshape(int w, int h)
{
	// get view port
	glViewport(0, 0, w, h);
	// get variables
	worldW = w;
	worldH = h;
	// set matrix mode
	glMatrixMode(GL_PROJECTION);
	// load identity
	glLoadIdentity();
	// algorithm
	gluOrtho2D(-0.05, width / height + 0.05, -0.05, 1);
	// push matrix mode
	glMatrixMode(GL_MODELVIEW);
	// replace the current matrix with the identity matrix
	glLoadIdentity();
}

// mouse function
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// get mouse position
		mouseX = x;
		mouseY = y;
		// convert to world coordinates
		mouseYinWorld = (1 - mouseY / worldH - 0.05) / 0.45 * 0.5;
		mouseXinWorld = (mouseX / worldW * (width / height + 0.05) - 0.1) * 1.15;
		// set color
		glColor3f(1.0, 1.0, 0);
		// set limitation and draw
		if (mouseYinWorld <= 0.5 && mouseYinWorld >= 0 && mouseXinWorld >= 0.1 && mouseXinWorld <= 1.1) {
			glBegin(GL_LINES);
			glVertex2f(0, mouseYinWorld);
			glVertex2f(mouseXinWorld, mouseYinWorld);
			glEnd();
			glBegin(GL_LINES);
			glVertex2f(mouseXinWorld, 0);
			glVertex2f(mouseXinWorld, mouseYinWorld);
			glEnd();
			// convert number to string
			ostringstream convert;   // stream used for the conversion
			convert << mouseYinWorld;      // insert the textual representation of 'Number' in the characters in the stream
			mouseYtext = convert.str();
			convert.str("");
			convert.clear();
			convert << mouseXinWorld;      // insert the textual representation of 'Number' in the characters in the stream
			mouseXtext = convert.str();
			convert.str("");
			convert.clear();

		}
		// flush and redisplay
		glFlush();
		glutPostRedisplay();
	}
}

void init(void)
{
	/* select clearing color 	*/
	glewInit();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// file menu Function
void fileMenuFunc(int id)
{
	// switch files
	switch (id)
	{
	case 1:
		currentFile = "normal.dat";
		break;
	case 2:
		currentFile = "expo.dat";
		break;
	case 3:
		currentFile = "1.dat";
		break;
	case 4:
		currentFile = "2.dat";
		break;
	case 5:
		currentFile = "3.dat";
		break;
	case 6:
		currentFile = "4.dat";
		break;
	case 7:
		currentFile = "5.dat";
		break;
	case 8:
		currentFile = "6.dat";
		break;
	case 9:
		currentFile = "7.dat";
		break;
	case 10:
		currentFile = "8.dat";
		break;
	case 11:
		currentFile = "9.dat";
		break;
	case 12:
		currentFile = "10.dat";
		break;
	case 13:
		currentFile = "11.dat";
		break;
	case 14:
		currentFile = "12.dat";
		break;
	case 15:
		currentFile = "13.dat";
		break;
	case 16:
		currentFile = "14.dat";
		break;
	case 17:
		currentFile = "15.dat";
		break;
	case 18:
		currentFile = "16.dat";
		break;
	case 19:
		currentFile = "17.dat";
		break;
	case 20:
		currentFile = "18.dat";
		break;

	case 21:
		currentFile = "19.dat";
		break;
	case 22:
		currentFile = "20.dat";
		break;
	}
	glutPostRedisplay();
}

// distribution menu function
void distributionMenuFunc(int id) {
	switch (id)
	{
	case 1:
		parameterStepText = "0.02";
		if (pdf) {
			pdfParameterStepSize = 0.02;
		}
		if (expo) {
			expoParameterStepSize = 0.02;
		}
		pdf = true;
		expo = false;
		break;
	case 2:
		if (pdf) {
			pdfParameterStepSize = 0.02;
		}
		if (expo) {
			expoParameterStepSize = 0.02;
		}
		parameterStepText = "0.02";
		expo = true;
		pdf = false;
		break;
	}
	glutPostRedisplay();
}

// histogram menu function
void histogramMenuFunc(int id) {
	switch (id)
	{
	case 1:
		numIntervals = 30;
		break;
	case 2:
		numIntervals = 40;
		break;
	case 3:
		numIntervals = 50;
		break;
	}
	glutPostRedisplay();
}

// parameter step menu function
void parameterStepMenuFunc(int id) {
	switch (id)
	{
	case 1:
		if (pdf) {
			pdfParameterStepSize = 0.01;
			ostringstream convert;
			convert << pdfParameterStepSize;
			parameterStepText = convert.str();
			convert.str("");
			convert.clear();
		}
		if (expo) {
			expoParameterStepSize = 0.01;
			ostringstream convert;
			convert << expoParameterStepSize;
			parameterStepText = convert.str();
			convert.str("");
			convert.clear();
		}
		break;
	case 2:
		if (pdf) {
			pdfParameterStepSize = 0.02;
			ostringstream convert;
			convert << pdfParameterStepSize;
			parameterStepText = convert.str();
			convert.str("");
			convert.clear();
		}
		if (expo) {
			expoParameterStepSize = 0.02;
			ostringstream convert;
			convert << expoParameterStepSize;
			parameterStepText = convert.str();
			convert.str("");
			convert.clear();
		}
		break;
	case 3:
		if (pdf) {
			pdfParameterStepSize = 0.05;
			ostringstream convert;
			convert << pdfParameterStepSize;
			parameterStepText = convert.str();
			convert.str("");
			convert.clear();
		}
		if (expo) {
			expoParameterStepSize = 0.05;
			ostringstream convert;
			convert << expoParameterStepSize;
			parameterStepText = convert.str();
			convert.str("");
			convert.clear();
		}
		break;
	}
	glutPostRedisplay();
}

// sub menu function
void subMenuFunc(int id) {
	switch (id)
	{
	case 1:
		exit(0);
		break;
	}
	glutPostRedisplay();
}

// create menu
void createMenu()
{
	int subMenu, fileSubMenu, distributionSubMenu, histogramSubMenu, parameterStepSubMenu;
	fileSubMenu = glutCreateMenu(fileMenuFunc);
	glutAddMenuEntry("normal.dat", 1);
	glutAddMenuEntry("expo.dat", 2);
	glutAddMenuEntry("1.dat", 3);
	glutAddMenuEntry("2.dat", 4);
	glutAddMenuEntry("3.dat", 5);
	glutAddMenuEntry("4.dat", 6);
	glutAddMenuEntry("5.dat", 7);
	glutAddMenuEntry("6.dat", 8);
	glutAddMenuEntry("7.dat", 9);
	glutAddMenuEntry("8.dat", 10);
	glutAddMenuEntry("9.dat", 11);
	glutAddMenuEntry("10.dat", 12);
	glutAddMenuEntry("11.dat", 13);
	glutAddMenuEntry("12.dat", 14);
	glutAddMenuEntry("13.dat", 15);
	glutAddMenuEntry("14.dat", 16);
	glutAddMenuEntry("15.dat", 17);
	glutAddMenuEntry("16.dat", 18);
	glutAddMenuEntry("17.dat", 19);
	glutAddMenuEntry("18.dat", 20);
	glutAddMenuEntry("19.dat", 21);
	glutAddMenuEntry("20.dat", 22);
	distributionSubMenu = glutCreateMenu(distributionMenuFunc);
	glutAddMenuEntry("Normal", 1);
	glutAddMenuEntry("Exponential", 2);
	histogramSubMenu = glutCreateMenu(histogramMenuFunc);
	glutAddMenuEntry("30", 1);
	glutAddMenuEntry("40", 2);
	glutAddMenuEntry("50", 3);
	parameterStepSubMenu = glutCreateMenu(parameterStepMenuFunc);
	glutAddMenuEntry("0.01", 1);
	glutAddMenuEntry("0.02", 2);
	glutAddMenuEntry("0.05", 3);
	subMenu = glutCreateMenu(subMenuFunc);
	glutAddSubMenu("File", fileSubMenu);
	glutAddSubMenu("Distribution", distributionSubMenu);
	glutAddSubMenu("Histogram", histogramSubMenu);
	glutAddSubMenu("Parameter Step", parameterStepSubMenu);
	glutAddMenuEntry("Exit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// main function
int main(int argc, char** argv)
{
	// check if program is running
	cout << "Program is running!" << endl;
	// initialize GLUT
	glutInit(&argc, argv);
	// initialize display mode
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	// set window size
	glutInitWindowSize(width, height);
	// set window position
	glutInitWindowPosition(100, 100);
	// create the window with a given title
	glutCreateWindow("Zelin's OpenGL Program");
	init();
	// check if program is initialized successfully
	cout << "Program initialized successfully!" << endl;
	// create menu
	createMenu();
	// check if menu is created
	cout << "Menu created succefully!" << endl;
	// set special function for arrow keys
	glutSpecialFunc(keyboard);
	// check if keyboard is set
	cout << "Keyboard is set!" << endl;
	// reshape window size
	glutReshapeFunc(reshape);
	// initialize mouse function
	glutMouseFunc(mouse);
	// display right now
	glutDisplayFunc(display);
	// clock start
	Timer(0);
	// check if it is in loop
	cout << "Now it is in a loop!" << endl;
	glutMainLoop();
	return 0;   /* ANSI C requires main to return int. */
}
