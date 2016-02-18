#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

#include <GL\glew.h>		// GLEW is "required" to use newer versions of OpenGL
#include <GL\freeglut.h>	// This example uses FreeGLUT for windowing

// GLM used for animation
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

#include "ShaderHelpers.h"
#include "Shape.h"
#include "Entity.h"

// Constants for our data size
const GLsizei numVertices = 18;	// The number of vertices we'll use for this demo
const GLsizei numComponents = numVertices * 2;	// The total amount of data (an x & y for each vertex)
GLuint shaderProgramIndex;

// set up Shape objects
Shape* shapeCircle;

// define Entities
Entity* entityPlayerPaddle;
Entity* entityOpponentPaddle;
Entity* entityBall;

// animation variables
int previousTime;
glm::vec3 currentVelocityOpponentPaddle;
glm::vec3 currentVelocityBall;

float ballScaleFactor;
int numberBounces; // the number of times the ball has bounced back and forth - alters speed
bool ballMissed;
bool opponentPaddleTrackMovement; // the opponent paddle should not start moving until after the first bounce of each round

// mouse position
glm::vec3 currentMousePosition;

// Other various initialization logic, so we don't bloat the main method
void init()
{
	// Set the color we'll clear our window to each frame
	glClearColor(0.100f, 0.294f, 0.560f, 1.0f); // Dark blue

	// Load the shaders
	shaderProgramIndex = loadShaderProgram(
		"Shaders/vertexShader.glsl", 
		"Shaders/fragmentShader.glsl");
	if( shaderProgramIndex != 0 )
		glUseProgram(shaderProgramIndex);

	// define a paddle - basically a circle
	GLfloat paddleVertexArray[] = {0.0f, 0.0f, 0.0f, 1.0f, .5f, .866f, 0.0f, 0.0f, .5f, .866f, .707f, .707f, 0.0f, 0.0f, // upper right quadrant
									.707f, .707f, .866f, .5f, 0.0f, 0.0f, .866f, .5f, 1.0f, 0.0f, 
						      0.0f, 0.0f, 0.0f, -1.0f, .5f, -.866f, 0.0f, 0.0f, .5f, -.866f, .707f, -.707f, 0.0f, 0.0f, // lower right: y vals negative
									.707f, -.707f, .866f, -.5f, 0.0f, 0.0f, .866f, -.5f, 1.0f, 0.0f, 
							  0.0f, 0.0f, 0.0f, -1.0f, -.5f, -.866f, 0.0f, 0.0f, -.5f, -.866f, -.707f, -.707f, 0.0f, 0.0f, // etc.
									-.707f, -.707f, -.866f, -.5f, 0.0f, 0.0f, -.866f, -.5f, -1.0f, 0.0f, 
							  0.0f, 0.0f, 0.0f, 1.0f, -.5f, .866f, 0.0f, 0.0f, -.5f, .866f, -.707f, .707f, 0.0f, 0.0f, // etc.
									-.707f, .707f, -.866f, .5f, 0.0f, 0.0f, -.866f, .5f, -1.0f, 0.0f};
	shapeCircle = new Shape(paddleVertexArray, 52, shaderProgramIndex);

	// set starting position of paddles
	entityPlayerPaddle = new Entity(shapeCircle, shaderProgramIndex, 0, 0, 0.5f, 0.5f);
	entityOpponentPaddle = new Entity(shapeCircle, shaderProgramIndex, 0, 0, 0.25f, 0.25f);
	// set starting velocity of opponent's paddle
	currentVelocityOpponentPaddle = glm::vec3(0, 0, 0);
	
	// initialize random seed
	srand( static_cast<unsigned int> (time(NULL)));
	// set starting position and scale factor of ball
	entityBall = new Entity(shapeCircle, shaderProgramIndex, 0, 0, .09f, .09f);
	ballScaleFactor = 1.0f;

	// set number of rallies to zero, ballMissed + opponent movement to false
	numberBounces = 0;
	ballMissed = false;
	opponentPaddleTrackMovement = false;
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// REMEMBER TO DRAW BG OBJECTS FIRST, FG OBJECTS LAST!
	// If the opponent missed the ball, draw it NOW
	if(ballMissed && ballScaleFactor <= 0.5f)
	{
		// define the color of the object (similar to player's paddle, but darker & minus transparency)
		setShaderColor(shaderProgramIndex, "colorHolder", 0.0f, 0.35f, 0.15f, 1.0f);
		// draw it - note that the ball is scaled based on times hit
		entityBall -> Draw();
	}

	// draw the opponent's paddle
	// define the color of the object
	setShaderColor(shaderProgramIndex, "colorHolder", 0.8f, 0.2f, 0.2f, 1.0f);
	// draw it
	entityOpponentPaddle -> Draw();

	// If in play, draw the ball NOW
	if(!ballMissed)
	{
		// define the color of the object
		setShaderColor(shaderProgramIndex, "colorHolder", 1.0f, 1.0f, 1.0f, 1.0f);
		// draw it
		entityBall -> Draw();
	}

	// draw the player's (transparent) paddle
	// define the color of the object
	setShaderColor(shaderProgramIndex, "colorHolder", 0.0f, 0.7f, 0.3f, 0.48f);
	// draw it
	entityPlayerPaddle -> Draw();

	// If the player missed the ball, draw it NOW
	if(ballMissed && ballScaleFactor >= 1.5f)
	{
		// define the color of the object (similar to opponent's paddle, but darker)
		setShaderColor(shaderProgramIndex, "colorHolder", 0.4f, 0.1f, 0.1f, 1.0f);
		// draw it
		entityBall -> Draw();
	}

	// Tell OpenGL to process the commands so far
	glFlush();
}

// The "Update" part of our game loop
void update()
{
	// Tells GLUT to redraw the window the next time
	// through the game loop
	glutPostRedisplay();

	// PLAYER MOVEMENT
	// player input for paddle location
	entityPlayerPaddle -> moveInst(currentMousePosition.x, currentMousePosition.y);

	// BALL MOVEMENT
	// ball changes direction if it hits the edge of the screen
	if(entityBall -> entityPosition.x <= -1.02 + (.09f * ballScaleFactor) || entityBall -> entityPosition.x >= 1.02 - (.09f * ballScaleFactor)) 
		{currentVelocityBall.x = -currentVelocityBall.x;}
	if(entityBall -> entityPosition.y <= -1.02 + (.09f * ballScaleFactor) || entityBall -> entityPosition.y >= 1.02 - (.09f * ballScaleFactor)) 
		{currentVelocityBall.y = -currentVelocityBall.y;}

	// scales the ball
	if(numberBounces == 0 && ballScaleFactor > 0.95f) {ballScaleFactor -= .00004f;} // This initial "slow period" gave the opponent paddle time to adjust
																				   // when a new round starts - the bug that made this necessary has since 
																				   // been fixed, but I liked the effect
	else if(numberBounces < 4 || ballMissed)
	{
		if((numberBounces % 2) == 0) {ballScaleFactor -= .0003f;} // ball gets smaller when game starts, or when player hit it last
		else {ballScaleFactor += .0003f;} // gets bigger when computer hit it last
	}
	// after five hits the ball speeds up (until it is missed)
	else
	{
		if((numberBounces % 2) == 0) {ballScaleFactor -= (.00015f * (numberBounces * 2 / 5));}
		else {ballScaleFactor += (.00015f * (numberBounces * 2 / 5));}
	}

	entityBall -> moveVelocity3D(currentVelocityBall.x, currentVelocityBall.y, ballScaleFactor);

	// If the ball has been missed, none of the following happens until the next round!
	if(!ballMissed)
	{
		// "Bounce" the ball when it "hits" the paddle - failure to do so leads to a miss.
		// PLAYER COLLISION CHECK + BOUNCING
		if(ballScaleFactor >= 1.5f) 
		{
			// Check for collision: If sum of radii > distance between center (i.e. coordinates) of each object, it is a miss
			// Determine distance between ball + paddle via pythagorean theorem
			// SEE IF THIS CODE CAN BE REWORKED
			float pythagInputX = entityBall -> entityPosition.x - entityPlayerPaddle -> entityPosition.x; pythagInputX *= pythagInputX;
			float pythagInputY = entityBall -> entityPosition.y - entityPlayerPaddle -> entityPosition.y; pythagInputY *= pythagInputY;
			float totalDistance = sqrt(pythagInputX + pythagInputY);

			// Check!
			if(totalDistance > 0.5f + (0.09f * ballScaleFactor)) {ballMissed = true;}
			// If the ball has not missed, proceed normally
			else
			{
				numberBounces += 1;
				// re-randomizes velocity on bounce
				if(numberBounces < 4)
				{
					currentVelocityBall.x = ((rand() % 3) - 1) * .0001f; currentVelocityBall.y = ((rand() % 3) - 1) * .0001f;
				}
				// after five hits the ball speeds up!
				else
				{
					currentVelocityBall.x = ((rand() % 3) - 1) * .00005f * (numberBounces * 2 / 5); 
					currentVelocityBall.y = ((rand() % 3) - 1) * .00005f * (numberBounces * 2 / 5);
				}
			}
		}

		// OPPONENT COLLISION CHECK + BOUNCING
		else if(ballScaleFactor <= 0.5f) 
		{
			// Check for collision: If sum of radii > distance between center (i.e. coordinates) of each object, it is a miss
			// Determine distance between ball + paddle via pythagorean theorem
			// THIS CODE ISN'T GREAT BECAUSE OF AN ERROR I DIDN'T UNDERSTAND??
			float pythagInputX = entityBall -> entityPosition.x - entityOpponentPaddle -> entityPosition.x; pythagInputX *= pythagInputX;
			float pythagInputY = entityBall -> entityPosition.y - entityOpponentPaddle -> entityPosition.y; pythagInputY *= pythagInputY;
			float totalDistance = sqrt(pythagInputX + pythagInputY);

			// Check!
			if(totalDistance > 0.25f + (0.09f * ballScaleFactor)) {ballMissed = true;}
			// If the ball has not missed, proceed normally
			else
			{
				numberBounces += 1;
				// re-randomizes velocity on bounce
				if(numberBounces < 4)
				{
					currentVelocityBall.x = ((rand() % 3) - 1) * .0001f; currentVelocityBall.y = ((rand() % 3) - 1) * .0001f;
				}
				// after five hits the ball speeds up!
				else
				{
					currentVelocityBall.x = ((rand() % 3) - 1) * .0001f * (numberBounces * 2 / 5); 
					currentVelocityBall.y = ((rand() % 3) - 1) * .0001f * (numberBounces * 2 / 5);
				}
			}
		}

		// OPPONENT MOVEMENT
		if(!opponentPaddleTrackMovement)
		{
			// Get the time elapsed
			int currentTime = glutGet(GLUT_ELAPSED_TIME);
			int dt = currentTime - previousTime;
			previousTime = currentTime; // thus, when update is called again...
		
			entityOpponentPaddle -> moveVelocitySpring(dt, entityBall, currentVelocityOpponentPaddle);
		}
		// Check to see if movement should start
		else if(numberBounces > 0) {opponentPaddleTrackMovement = true;}
	}
	// If ball HAS missed, wait until it reaches a certain scale before resetting it
	else if(ballScaleFactor >= 2.0f || ballScaleFactor <= .01f)
	{
		// NEW ROUND!
		// Remember to reset the opponent's position, too, or it'll freak out!
		entityOpponentPaddle -> entityPosition = glm::vec3(0, 0, 0);
		
		// set starting position and scale factor of ball
		entityBall -> entityPosition = glm::vec3(0, 0, 0);
		ballScaleFactor = 1.0f;

		// set number of rallies to zero, ballMissed to false, velocity to null
		numberBounces = 0;
		ballMissed = false;
		currentVelocityBall.x = 0; currentVelocityBall.y = 0;
	}
}

// Handles mouse input
void mouseMove(int thisX, int thisY)
{
	// convert parameters to match up w/ openGL
	currentMousePosition.x = (2 * (thisX / 800.0f)) - 1; // changes x from pixels to the [-1, 1] range
	currentMousePosition.y = -1 * ((2 * (thisY / 600.0f)) - 1); // changes y similarly, AND NEGATES IT since OpenGL's y axis is inverted
}

// Main - with command line stuff, since glut wants it
int main(int argc, char** argv)
{
	// Set up OpenGL via glut
	glutInit(&argc, argv);						// Pass args to glut
	glutInitDisplayMode(GLUT_RGBA);				// Set up the window display mode
	glutInitWindowSize(700, 700);				// Set up the window size
	glutInitContextVersion(3, 1);				// Which version of OpenGL to use (4.3 is what the book covers)
	glutInitContextProfile(GLUT_CORE_PROFILE);	// Core since we don't need backwards compatibility
	glutCreateWindow("The Window Title!");		// Create the window with a specific title
	
	// Initialize GLEW and exit if it fails
	if(glewInit() != GLEW_OK) // GLEW_OK is a #define (which equals 0)
	{
		return -1;
	}

	// Tell glut which functions to call as part
	// of the main loop
	glutIdleFunc(update);
	glutDisplayFunc(draw);
	glutPassiveMotionFunc(mouseMove);

	// Needed for use of transparency/Alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );

	// makes cursor invisible
	glutSetCursor(GLUT_CURSOR_NONE);

	// Handle any other initialization logic
	init();

	// Run the main loop
	glutMainLoop();
}