#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>

#include "external-files/glew/glew.h"
#include <GL/glut.h>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "external-files/glm/glm.hpp"
#include "external-files/glm/gtc/random.hpp"

#include "camera.h"
#include "particles.h"
#include "shader.h"

using namespace std;

// Window dimentions
unsigned int winX = 800, winY = 600;

Particles *particles;
// KEEP THE SAME AS STATED IN COMPUTE SHADER
const unsigned int blockSize = 256;

// Lower constant is performance sucks
const unsigned int numParticles = blockSize * 50;

Camera camera(winX, winY);

// Various state variables
float prevFrameTime = 0.0;
bool paused = false;
bool points = false;
bool wireframe = false;
bool blend = true;
float timeStep = 0.02;

// Particle initialisers
glm::vec4 posW() {
	return glm::vec4(glm::ballRand(60.0),
					 glm::max(0.2f, glm::gaussRand(1.0f, 0.5f)));
}
glm::vec4 posW0() { return glm::vec4(0,0,0,1); }
glm::vec4 velO() { return glm::vec4(glm::sphericalRand(0.3f), 0.0); }
glm::vec4 velO1() { return glm::vec4(0.0); }

void init()
{
	// Seed randoms
	srand(time(NULL));
	//srand(42);

	// Set up particles
	particles = new Particles("data/octahedron/octahedron.obj",
							  "data/octahedron/",
							  blockSize,
							  numParticles);
	particles->addParticles(numParticles, posW, velO);

	// Set up camera
	camera.setPos(0.0, 0.0, 100.0);
	camera.setPerp(45.0, 0.1, 4000);

	// Set background color
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	particles->draw(camera, points);

	glutSwapBuffers();
}

void idle()
{
	// Calculate the elapsed time and update camera
	float now = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	camera.update(now - prevFrameTime + 0.001);

	// Reset previous frames time
	prevFrameTime = now;

	// Note: simulation runs independent of time elapsed
	if (!paused)
		particles->step(timeStep);

	glutPostRedisplay();
}

void reshape(int x, int y)
{
	winX = x;
	winY = y;

	camera.reshape(x, y);

	// Move cursor to middle of screen
	glutWarpPointer(x / 2, y / 2);
	glViewport(0, 0, x, y);
}

void free()
{
	delete particles;
}

void onKeyDown(unsigned char key, int x, int y)
{
	switch (key) {
		// End program
		case 033: // Esc
			free();
			exit(0);
			break;

		// Camera movement
		case 'w':
		case 'W':
			camera.forward = true;
			break;
		case 's':
		case 'S':
			camera.back = true;
			break;
		case 'd':
		case 'D':
			camera.right = true;
			break;
		case 'a':
		case 'A':
			camera.left = true;
			break;
		case '.':
		case '>':
			camera.up = true;
			break;
		case ',':
		case '<':
			camera.down = true;
			break;
		case '/':
		case '?':
			camera.slow = true;
			break;

		// Change simulator timestep
		// Note: changing timestep changes how much particles attract
		case ' ':
			paused = !paused;
			break;
		case '=':
		case '+':
			timeStep *= 1.1;
			break;
		case '-':
		case '_':
			timeStep /= 1.1;
			break;
		case '0':
		case ')':
			// Reset timestep
			timeStep = 0.02;
			break;

		// Render opttions
		case 'p':
		case 'P':
			points = !points;
			break;
		case 'q':
		case 'Q':
			if (wireframe) {
				glPolygonMode(GL_FRONT, GL_FILL);
				glEnable(GL_CULL_FACE);
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
			}

			wireframe = !wireframe;
			break;
		case 'b':
		case 'B':
			if (blend) {
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			} else {
				glEnable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
			}

			blend = !blend;
			break;
	}
}

void onKeyUp(unsigned char key, int x, int y)
{
	switch (key) {
		case 'w':
		case 'W':
			camera.forward = false;
			break;
		case 's':
		case 'S':
			camera.back = false;
			break;
		case 'd':
		case 'D':
			camera.right = false;
			break;
		case 'a':
		case 'A':
			camera.left = false;
			break;
		case '.':
		case '>':
			camera.up = false;
			break;
		case ',':
		case '<':
			camera.down = false;
			break;
		case '/':
		case '?':
			camera.slow = false;
			break;
	}
}

int px = 0, py = 0;
bool firstMoveMove = true;
void mouseMove(int x, int y)
{
	// Skip first mouse input as mouse must be centered first
	if (firstMoveMove) {
		firstMoveMove = false;
		return;
	}

	// Calculate distance from center
	int dx = x - (winX / 2);
	int dy = y - (winY / 2);

	// Update camera and move pointer back to center
	if (dx || dy) {
		camera.look(dx, dy);
		glutWarpPointer(winX / 2, winY / 2);
	}
}

int main(int argc, char* argv[])
{
	// Initialse window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(winX, winY);
	glutCreateWindow("NBody");
	glutFullScreen();

	// Setup input
	glutIgnoreKeyRepeat(true);
	glutSetCursor(GLUT_CURSOR_NONE);

	// Wrangle OpenGL
	glewInit();
	if (!GLEW_VERSION_4_3) {
		cout << "No support for OpenGL 4.3" << endl;
		//return 1;
	}

	// Set GL properties
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	init();

	// Set callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutPassiveMotionFunc(mouseMove);

	glutMainLoop();
	return 0;
}
