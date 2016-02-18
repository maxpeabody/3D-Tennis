#pragma once

#include <GL\glew.h>		// GLEW is "required" to use newer versions of OpenGL
#include <GL\freeglut.h>

class Shape
{
public:
	Shape(GLfloat*vertexArray, int thisNumVertices, GLuint thisShaderProgIndex);
	void Draw(float x, float y, float xScale, float yScale);
	~Shape(void);

	// infrastructure
	GLuint shapeVertexBuffer; 
	GLuint shapeVAO;
	int totalVertices;
	GLuint shaderProgramIndex;

	// offset and scaling
	GLint shapeOffset;
	GLint shapeScaleFactor;
};

