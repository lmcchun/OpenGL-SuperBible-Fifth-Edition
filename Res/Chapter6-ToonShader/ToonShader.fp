// 动画光照着色器
// 片段着色器

#version 130

uniform sampler1D colorTable;

out vec4 vFragColor;

smooth in float textureCoordinate;

void main(void)
{ 
	vFragColor = texture(colorTable, textureCoordinate);
}