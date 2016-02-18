#version 330

layout (location = 0) in vec2 position;
uniform vec2 shaderOffset, shaderScale;

void main()
{
	gl_Position = vec4(position * shaderScale + shaderOffset, 0, 1);
}