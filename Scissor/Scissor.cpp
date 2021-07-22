// Out first OpenGL program that will just draw a triangle on the screen.

#include "GLTools.h" // OpenGL toolkit
#include "GLShaderManager.h" // Shader Manager Class

#ifdef __APPLE__
#include <glut/glut.h> // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h> // Windows FreeGLUT equivalent
#endif

// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}

///////////////////////////////////////////////////////////////////////////////

// Called to draw scene
void RenderScene()
{
	// Clear blue window
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f); // 为什么不设置 alpha 值
	glClear(GL_COLOR_BUFFER_BIT);

	// Now set scissor to smaller red sub region
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glScissor(100, 100, 600, 400);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	// Finally, an even smaller green rectangle
	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glScissor(200, 200, 400, 200);
	glClear(GL_COLOR_BUFFER_BIT);

	// Turn scissor back off for next render
	glDisable(GL_SCISSOR_TEST);

	// Perform the buffer swap to display the back buffer
	glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////

// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL Scissor");

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);

	glutMainLoop();

	return 0;
}