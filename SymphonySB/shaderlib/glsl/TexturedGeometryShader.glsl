#section VERTEX_SHADER

layout(location = 0) in vec3 attribute_Position;
layout(location = 1) in vec3 attribute_Color;
layout(location = 2) in vec2 attribute_TexCoords;

out vec3 pass_Color;
out vec2 pass_TexCoords;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 Transform;

void main()
{
	pass_Color = attribute_Color;
    pass_TexCoords = attribute_TexCoords;
	gl_Position = vec4(attribute_Position, 1.0) * ProjectionMatrix * ViewMatrix * Transform;
}

#section FRAGMENT_SHADER

layout(location = 0) out vec4 out_Color;

in vec3 pass_Color;
in vec2 pass_TexCoords;

uniform int useUniformColor;
uniform sampler2D TextureID;
uniform vec3 uniformGeometryColor;

void main()
{
	if (useUniformColor == 1)
	{
		gl_FragColor = vec4(uniformGeometryColor, 1.0) * texture(TextureID, pass_TexCoords.xy);
	}
	else
	{
		gl_FragColor = vec4(pass_Color, 1.0) * texture(TextureID, pass_TexCoords.xy);
	}
}