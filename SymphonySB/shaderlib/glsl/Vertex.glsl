#version 450

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

out vec3 passColor;

layout (std140, binding = 0) uniform RendererUniforms
{
	mat4 Projection;
	mat4 View;
	mat4 Model;
};

void main()
{
	passColor = Color;
	gl_Position = Projection * View * Model * vec4(Position, 1.0);
}