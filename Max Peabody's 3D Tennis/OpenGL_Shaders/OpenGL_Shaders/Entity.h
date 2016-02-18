#pragma once

#include <GL\glew.h>		// GLEW is "required" to use newer versions of OpenGL
#include <GL\freeglut.h>	// This example uses FreeGLUT for windowing

// GLM used for animation
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

#include "ShaderHelpers.h"
#include "Shape.h"

class Entity
{
public:
	Entity(Shape*thisShape, GLuint thisShaderProgram, float startingLocX, float startingLocY, float scaleX, float scaleY);

	void moveInst(float thisX, float thisY);
	void moveVelocity3D(float velocityX, float velocityY, float scaleFactorGiven);
	void moveVelocitySpring(int dt, Entity*targetEntity, glm::vec3 givenVelocity);
	void Draw(void);
	void Update(void);

	~Entity(void);

	GLuint shaderProgramIndex;
	Shape*entityShape;
	glm::vec3 entityPosition;
	glm::vec3 entityScale;
};

