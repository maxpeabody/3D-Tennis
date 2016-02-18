#include "ShaderHelpers.h"

#include <fstream>
#include <iostream>

using namespace std;

// Loads the text from a file and returns it as
// a single c-style string
char* loadTextFile(const char* file)
{
	ifstream inFile(file, ios::binary);
	if( !inFile.is_open() )
		return 0;

	// Calc size
	inFile.seekg(0, ios::end);
	int length = (int)inFile.tellg();
	inFile.seekg(0, ios::beg);

	// Read data
	char* fileContents = new char[length + 1];
	inFile.read(fileContents, length);
	fileContents[length] = 0;

	// All done
	inFile.close();
	return fileContents;
}

// Loads a shader of the specified type
GLuint loadShader(const char* file, GLenum shaderType)
{
	// Get the file contents
	const char* fileContents = loadTextFile(file);
	if( !fileContents )
	{
		cout << "Error reading file: " << file << endl;
		delete [] fileContents;
		return 0;
	}

	// Get an unused shader index
	GLuint shader = glCreateShader(shaderType);

	// Set the source (glsl code) of the shader
	glShaderSource(
		shader,			// Which shader is getting the code
		1,				// How many strings make up the next parameter
		&fileContents,	// Where the actual text of the shader is stored
		0);				// The lengths of each string in the shader source - zero means "they're null terminated"

	// Delete the text
	delete [] fileContents;

	// Compile the shader
	glCompileShader(shader);

	// Did it work?
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if( compiled == GL_TRUE )
	{
		// Compilation was successful!
		return shader;
	}
	else
	{
		// Problem compiling! We'll need the shader log, but first we
		// need the length of the log (includes null terminator)
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		// Create a buffer to hold the log
		char* log = new char[logLength];
		glGetShaderInfoLog(
			shader,			// The shader whose log to retrieve
			logLength,		// The number of characters to retrieve
			0,				// Pointer to an int where the number of characters will be reported - or null if we already know it
			log);			// The character array to put the log into

		// Write to the console and clean up
		cout << "Error compiling shader:\n" << log << endl;
		glDeleteShader(shader);		// Free the shader
		delete [] log;				// Delete the c-style string
		return 0;					// Return null
	}
}

// Loads and combines multiple shaders into a shader program
GLuint loadShaderProgram(const char* vertexFile, const char* fragmentFile)
{
	// Load the vertex shader
	GLuint vertexShader = loadShader(vertexFile, GL_VERTEX_SHADER);
	if( vertexShader == 0 )
	{
		// There was a problem - Can't continue.  Return zero.
		return 0;
	}

	// Load the pixel shader
	GLuint fragmentShader = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
	if( fragmentShader == 0 )
	{
		// Problem - can't continue.  Return zero.
		return 0;
	}

	// If we got here, both shaders compiled correctly - Create a program
	GLuint shaderProgram = glCreateProgram();

	// Attach each shader to the program and link them
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Did it work?
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
	if( linked == GL_TRUE )
	{
		// Program linked successfully!
		return shaderProgram;
	}
	else
	{
		// Problem linking program!  Get the log
		GLint logLength;
		glGetProgramiv(
			shaderProgram,			// Which program?
			GL_INFO_LOG_LENGTH,		// Getting the log length
			&logLength);			// Variable to store the length

		// Create a buffer to hold the length
		char* log = new char[logLength];
		glGetProgramInfoLog(
			shaderProgram,		// Which program?
			logLength,			// The length to get
			0,					// Pointer to an int where the number of characters will be reported - or null if we already know it
			log);				// Where to put the log data

		// Print and clean up
		cout << "Error linking shaders:\n" << log << endl;
		glDeleteProgram(shaderProgram);
		delete [] log;
		return 0;
	}
}

void setShaderColor(GLuint shaderProgIndex, const char* uniformVarName, float redValue, float greenValue, float blueValue, float alphaValue)
{
	// Note: A real game engine wouldn't do this every time it needed the uniform variable, but here it's okay
	GLuint uniformLocIndex = glGetUniformLocation(shaderProgIndex, uniformVarName); // gets location of uniformVarName in shaderProgIndex
	glProgramUniform4f(shaderProgIndex, uniformLocIndex, redValue, greenValue, blueValue, alphaValue); 
}