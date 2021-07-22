// Demonstrates a simple flat shader with transformations

#include <math.h>
#include "GLTools.h" // OpenGL Toolkit
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include <StopWatch.h>

#ifdef __APPLE__
#include <glut/glut.h> // OS x version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrame				viewFrame;
GLFrustum			viewFrustum;
GLTriangleBatch		torusBatch;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
GLGeometryTransform	transformPipeline;

GLuint	flatShader;	// The Flat Shader

GLint	locMVP;		// The location of the ModelViewProjection matrix uniform
GLint	locColor;	// The location of the color value uniform

// This function does any needed initialization on the rendering context.
void SetupRC(void)
{
	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	viewFrame.MoveForward(4.0f);

	// Make the torus
	gltMakeTorus(torusBatch, 0.80f, 0.25f, 52, 26);

	const char* szVertexProgFileName = "../Res/Chapter6-FlatShader/FlatShader.vp";
	const char* szFragmentProgFileName = "../Res/Chapter6-FlatShader/FlatShader.fp";
	flatShader = gltLoadShaderPairWithAttributes(szVertexProgFileName, szFragmentProgFileName, 1, GLT_ATTRIBUTE_VERTEX, "vVertex");

	locMVP = glGetUniformLocation(flatShader, "mvpMatrix");
	locColor = glGetUniformLocation(flatShader, "vColorValue");
}

// Cleanup
void ShutdownRC(void)
{
	//
}

// Called to draw scene
void RenderScene(void)
{
	static CStopWatch rotTimer;

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelViewMatrix.PushMatrix(viewFrame);
	modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 10.0f, 0.0f, 1.0f, 0.0f);

	GLfloat vColor[] = { 0.1f, 0.1f, 1.0f, 1.0f };

	glUseProgram(flatShader);
	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniform4fv(locColor, 1, vColor);

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

	// Set viewport to window dimensions
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
	glutCreateWindow("Simple, Transform, Geometry");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();
	glutMainLoop();
	ShutdownRC();

	return 0;
}
