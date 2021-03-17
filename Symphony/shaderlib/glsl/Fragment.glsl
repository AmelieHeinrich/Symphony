#version 450

layout (location = 0) out vec4 Color;

in vec3 passColor;

void main()
{
	Color = vec4(passColor, 1.0);
}