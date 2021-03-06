// The TexturedTriangle Shader
// Fragment Shader

#version 130

uniform sampler2D colorMap;

in vec2 vVaryingTexCoords;

out vec4 vFragColor;

void main(void)
{
	vFragColor = texture(colorMap, vVaryingTexCoords);
}