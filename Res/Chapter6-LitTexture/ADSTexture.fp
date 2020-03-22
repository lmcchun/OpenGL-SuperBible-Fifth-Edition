// ADS 点光照着色器
// 片段着色器

#version 130

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec3 specularColor;
uniform sampler2D colorMap;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vTexCoords;

out vec4 vFragColor;

void main(void)
{ 
	vec3 vVaryingNormal1 = normalize(vVaryingNormal);
	vec3 vVaryingLightDir1 = normalize(vVaryingLightDir);
	// 从点乘积得到漫反射强度
	float diffuse = max(0.0, dot(vVaryingNormal1, vVaryingLightDir1));
	// 用强度乘以漫反射颜色, 将 alpha 值设为 1.0
	vFragColor.rgb = diffuse * diffuseColor.rgb;
	vFragColor.a = diffuseColor.a;
	// 添加环境光
	vFragColor += ambientColor;
	// Modulate in the texture
	vFragColor *= texture(colorMap, vTexCoords); // 是否考虑将此项放到漫反射中计算?
	// 镜面光
	vec3 vReflection = normalize(reflect(-vVaryingLightDir1, vVaryingNormal1));
	float specular = max(0.0, dot(vVaryingNormal1, vReflection));
	// 如果漫射光为 0, 那么就不必考虑指数函数了
	if (diffuse != 0)
	{
		float fSpecular = pow(specular, 128.0);
		vFragColor.rgb += specularColor * fSpecular;
	}
}