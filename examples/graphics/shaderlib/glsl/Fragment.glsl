#version 450

layout (location = 0) out vec4 Color;

in vec3 passColor;

uniform sampler2D currentTexture;
in vec2 passTexCoords;

void main()
{
	Color = texture(currentTexture, passTexCoords);
}