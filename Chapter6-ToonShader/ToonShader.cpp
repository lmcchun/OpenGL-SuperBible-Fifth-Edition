#include <GLTools.h> // OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrame             viewFrame;
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;

GLuint	toonShader;		// The dissolving light shader
GLint	locLight;		// The location of the Light in eye coordinates
GLint	locMVP;			// The location of the ModelViewProjection matrix uniform
GLint	locMV;			// The location of the ModelView matrix uniform
GLint	locNM;			// The location of the Normal matrix uniform
GLint   locColorTable;	// The location of the color table

GLuint texture;

// 这个函数能够在渲染环境中进行任何重要的初始化
void SetupRC(void)
{
	// 背景
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	shaderManager.InitializeStockShaders();
	viewFrame.MoveForward(4.0f);
	// Make the torus
	gltMakeTorus(torusBatch, 0.8f, 0.25f, 52, 26);
	const char* szVertexProgFileName = "../Res/Chapter6-ToonShader/ToonShader.vp";
	const char* szFragmentProgFileName = "../Res/Chapter6-ToonShader/ToonShader.fp";
	toonShader = gltLoadShaderPairWithAttributes(szVertexProgFileName, szFragmentProgFileName, 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
	locLight = glGetUniformLocation(toonShader, "vLightPosition");
	locMVP = glGetUniformLocation(toonShader, "mvpMatrix");
	locMV = glGetUniformLocation(toonShader, "mvMatrix");
	locNM = glGetUniformLocation(toonShader, "normalMatrix");
	locColorTable = glGetUniformLocation(toonShader, "colorTable");

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_1D, texture);
	GLubyte textureData[4][3] = {
		32,  0, 0,
		64,  0, 0,
		128, 0, 0,
		255, 0, 0
	};

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

// Cleanup
void ShutdownRC(void)
{
	glDeleteProgram(toonShader);
	glDeleteTextures(1, &texture);
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
	GLfloat vDiffuseColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUseProgram(toonShader);
	glUniform3fv(locLight, 1, vEyeLight);
	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
	glUniform1i(locColorTable, 0);
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
	glutCreateWindow("Cell (toon) shading");
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