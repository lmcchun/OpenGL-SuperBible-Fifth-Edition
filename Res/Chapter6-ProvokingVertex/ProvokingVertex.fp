// ProvokingVertex flat shader demo
// Fragment Shader

#version 130

flat in vec4 vVaryingColor;

out vec4 vFragColor;

void main(void)
{
	vFragColor = vVaryingColor;
}