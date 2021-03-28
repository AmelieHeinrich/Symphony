#version 450

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 TexCoords;

out vec3 passColor;
out vec2 passTexCoords;

layout (std140, binding = 0) uniform RendererUniforms
{
	mat4 Projection;
	mat4 View;
	mat4 Model;
};

void main()
{
	passColor = Color;
	passTexCoords = TexCoords;
	gl_Position = Model * View * Projection * vec4(Position, 1.0);
}