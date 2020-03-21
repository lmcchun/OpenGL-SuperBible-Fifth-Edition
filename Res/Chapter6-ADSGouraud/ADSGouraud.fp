// Simple Diffuse lighting Shader

#version 130

smooth in vec4 vVaryingColor;

out vec4 vFragColor;

void main(void)
{ 
	vFragColor = vVaryingColor;
}