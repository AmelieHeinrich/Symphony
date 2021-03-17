#version 450

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

out vec3 passColor;

void main()
{
	passColor = Color;
	gl_Position = vec4(Position, 1.0);
}