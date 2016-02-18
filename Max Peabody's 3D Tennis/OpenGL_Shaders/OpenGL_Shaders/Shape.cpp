#include "Shape.h"


Shape::Shape(GLfloat*vertexArray, int thisNumVertices, GLuint thisShaderProgIndex)
{
	totalVertices = thisNumVertices; this -> shaderProgramIndex = thisShaderProgIndex;

	// Generate a Vertex Array Object (VAO), then bind it (making it the active one)
	// This effectively "records" several later OpenGL calls to the VAO
	glGenVertexArrays(1, &shapeVAO);	// How many, and a pointer to the first one
	glBindVertexArray(shapeVAO);

	// Generate an OpenGL buffer, then bind it (making it the active buffer)
	// This will be "recorded" into our VAO
	glGenBuffers(1, &shapeVertexBuffer);	// How many, and a pointer to the first one
	glBindBuffer(
		GL_ARRAY_BUFFER,	// The type of buffer (this is a buffer holding raw vertex data)
		shapeVertexBuffer);			// The actual buffer to bind

	// Put data into the currently bound buffer (VBO)
	glBufferData(
		GL_ARRAY_BUFFER,					// A buffer holding raw data
		sizeof(GL_FLOAT) * totalVertices * 2, // size of array in bytes?
		vertexArray,							// The actual data
		GL_STATIC_DRAW);					// This data will be set once and used multiple times

	// "Set up and enable an appropriate vertex attribute"??????

	// Tell OpenGL about the layout of our vertex data, which is just a bunch of "positions"
	glVertexAttribPointer(
		0,			// The index of the attribute - 0 since it's the first one we've created
		2,			// The number of components for this attribute - Every 2 numbers is a single "position" (x & y)
		GL_FLOAT,	// The type of data, so OpenGL knows how big each component is
		GL_FALSE,	// We don't want our data to be normalized
		0,			// The stride (in bytes) between each of these attributes - Zero since our data is several positions in a row
		0);			// An offset to the beginning of the first one of these attributes

	// Enable this layout (which will be recorded in the VAO)
	glEnableVertexAttribArray(0);

	// retrieve offset and scale factor from shader program
	shapeOffset = glGetUniformLocation(shaderProgramIndex, "shaderOffset");
	shapeScaleFactor = glGetUniformLocation(shaderProgramIndex, "shaderScale");
}

// actually draw the shape to the screen
void Shape::Draw(float x, float y, float xScale, float yScale)
{
	// Send offset and scale factor to the appropriate uniform variable
	glProgramUniform2f(shaderProgramIndex, shapeOffset, x, y);
	glProgramUniform2f(shaderProgramIndex, shapeScaleFactor, xScale, yScale);

	// Draw the currently bound data
	glBindVertexArray(shapeVAO);
	glDrawArrays(GL_TRIANGLES, 0, totalVertices);
}

Shape::~Shape(void)
{
	// delete VAO and VBO
	glDeleteVertexArrays(1, &shapeVAO);
	glDeleteBuffers(1, &shapeVertexBuffer);
}
