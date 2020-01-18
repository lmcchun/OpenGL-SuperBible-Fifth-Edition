// The ShadedIdentity Shader
// Fragment Shader

#version 130

in vec4 vVaryingColor;

out vec4 vFragColor;

void main(void)
{
	vFragColor = vVaryingColor;
}