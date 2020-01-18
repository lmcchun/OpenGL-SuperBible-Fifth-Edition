// Flat Shader
// Fragment Shader

#version 130

// Make geometry solid
uniform vec4 vColorValue;

// Output fragment color
out vec4 vFragColor;

void main(void)
{
	vFragColor = vColorValue;
}