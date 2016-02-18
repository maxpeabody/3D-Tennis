#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

// Loads the text from a file and returns it as
// a single c-style string
char* loadTextFile(const char* file);

// Loads a shader of the specified type
GLuint loadShader(const char* file, GLenum shaderType);

// Loads and combines multiple shaders into a shader program
GLuint loadShaderProgram(const char* vertexFile, const char* fragmentFile);

void setShaderColor(GLuint shaderProgIndex, const char* uniformVarName, float redValue, float greenValue, float blueValue, float alphaValue);
void setShaderVec2(GLuint shaderProgIndex, const char* uniformVarName, glm::vec2 vecToSend);
void setShaderMatrix(GLuint shaderProgIndex, const char* uniformVarName, glm::mat4 matrixToSend);