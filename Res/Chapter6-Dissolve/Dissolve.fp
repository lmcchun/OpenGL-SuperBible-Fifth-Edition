// ADS �������ɫ��
// Ƭ����ɫ��

#version 130

uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec3 specularColor;
uniform sampler2D  cloudTexture;
uniform float dissolveFactor;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vVaryingTexCoord;

out vec4 vFragColor;

void main(void)
{ 
	vec4 vCloudSample = texture(cloudTexture, vVaryingTexCoord);
	if (vCloudSample.r < dissolveFactor)
	{
		discard;
	}
	vec3 vVaryingNormal1 = normalize(vVaryingNormal);
	vec3 vVaryingLightDir1 = normalize(vVaryingLightDir);
	// �ӵ�˻��õ�������ǿ��
	float diffuse = max(0.0, dot(vVaryingNormal1, vVaryingLightDir1));
	// ��ǿ�ȳ�����������ɫ, �� alpha ֵ��Ϊ 1.0
	vFragColor.rgb = diffuse * diffuseColor.rgb;
	vFragColor.a = diffuseColor.a;
	// ��ӻ�����
	vFragColor += ambientColor;
	// �����
	vec3 vReflection = normalize(reflect(-vVaryingLightDir1, vVaryingNormal1));
	float specular = max(0.0, dot(vVaryingNormal1, vReflection));
	// ��������Ϊ 0, ��ô�Ͳ��ؿ���ָ��������
	if (diffuse != 0)
	{
		float fSpecular = pow(specular, 128.0);
		vFragColor.rgb += specularColor * fSpecular;
	}
}