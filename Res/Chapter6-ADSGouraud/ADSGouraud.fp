// ADS 点光照着色器
// 片段着色器

#version 130

smooth in vec4 vVaryingColor;

out vec4 vFragColor;

void main(void)
{ 
	vFragColor = vVaryingColor;
}