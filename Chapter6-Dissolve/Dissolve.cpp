#include "Common.h"

#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <StopWatch.h>

#include <math.h>


GLFrame             viewFrame;
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

GLuint	ADSDissloveShader;	// The diffuse light shader
GLint	locAmbient;			// The location of the ambient color
GLint   locDiffuse;			// The location of the diffuse color
GLint   locSpecular;		// The location of the specular color
GLint	locLight;			// The location of the Light in eye coordinates
GLint	locMVP;				// The location of the ModelViewProjection matrix uniform
GLint	locMV;				// The location of the ModelView matrix uniform
GLint	locNM;				// The location of the Normal matrix uniform
GLint   locTexture;			// The location of the texture uniform
GLint   locDissolveFactor;  // The location of the dissolve factor

GLuint  cloudTexture;		// The cloud texture texture object


// 这个函数能够在渲染环境中进行任何重要的初始化
void SetupRC(void)
{
	// 背景
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(4.0f);
	// Make the torus
	gltMakeTorus(torusBatch, 0.8f, 0.25f, 52, 26);
	const char* szVertexProgFileName = "../Res/Chapter6-Dissolve/Dissolve.vp";
	const char* szFragmentProgFileName = "../Res/Chapter6-Dissolve/Dissolve.fp";
	ADSDissloveShader = gltLoadShaderPairWithAttributes(
		szVertexProgFileName, szFragmentProgFileName, 3,
		GLT_ATTRIBUTE_VERTEX, "vVertex",
		GLT_ATTRIBUTE_NORMAL, "vNormal",
		GLT_ATTRIBUTE_TEXTURE0, "vTexCoords0");
	locAmbient = glGetUniformLocation(ADSDissloveShader, "ambientColor");
	locDiffuse = glGetUniformLocation(ADSDissloveShader, "diffuseColor");
	locSpecular = glGetUniformLocation(ADSDissloveShader, "specularColor");
	locLight = glGetUniformLocation(ADSDissloveShader, "vLightPosition");
	locMVP = glGetUniformLocation(ADSDissloveShader, "mvpMatrix");
	locMV = glGetUniformLocation(ADSDissloveShader, "mvMatrix");
	locNM = glGetUniformLocation(ADSDissloveShader, "normalMatrix");
	locTexture = glGetUniformLocation(ADSDissloveShader, "cloudTexture");
	locDissolveFactor = glGetUniformLocation(ADSDissloveShader, "dissolveFactor");

	glGenTextures(1, &cloudTexture);
	glBindTexture(GL_TEXTURE_2D, cloudTexture);
	LoadTGATexture("../Res/Chapter6-Dissolve/Clouds.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
}

// Cleanup
void ShutdownRC(void)
{
	glDeleteProgram(ADSDissloveShader);
	glDeleteTextures(1, &cloudTexture);
}

// 进行调用以绘制场景
void RenderScene(void)
{
	static CStopWatch rotTimer;

	// 清除窗口和深度缓冲区
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelViewMatrix.PushMatrix(viewFrame);
	modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 10.0f, 0.0f, 1.0f, 0.0f);

	GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
	GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat vDiffuseColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f };

	glBindTexture(GL_TEXTURE_2D, cloudTexture);

	glUseProgram(ADSDissloveShader);
	glUniform4fv(locAmbient, 1, vAmbientColor);
	glUniform4fv(locDiffuse, 1, vDiffuseColor);
	glUniform3fv(locSpecular, 1, vSpecularColor);
	glUniform3fv(locLight, 1, vEyeLight);
	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
	glUniform1i(locTexture, 0); // XXX 纹理单元 0

	float fFactor = (float)fmod(rotTimer.GetElapsedSeconds(), 10.0f) / 10.0f;
	glUniform1f(locDissolveFactor, fFactor);

	torusBatch.Draw();

	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void ChangeSize(int w, int h)
{
	// Prevent a divide by zero
	if (h == 0)
	{
		h = 1;
	}

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(35.0f, (float)w / (float)h, 1.0f, 100.0f);

	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Help me, I'm melting!");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();
	glutMainLoop();
	ShutdownRC();

	return 0;
}