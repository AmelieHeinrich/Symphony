#version 450

out vec4 Color;

in vec3 passColor;

uniform sampler2D RendererTextures[10];
uniform int TextureSlot;
in vec2 passTexCoords;

void main()
{
	Color = texture(RendererTextures[TextureSlot], passTexCoords);
}