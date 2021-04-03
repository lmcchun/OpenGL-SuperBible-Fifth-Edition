// Out first OpenGL program that will just draw a triangle on the screen.

#include <GLTools.h> // OpenGL toolkit
#include <GLShaderManager.h> // Shader Manager Class

#ifdef __APPLE__
#include <glut/glut.h> // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h> // Windows FreeGLUT equivalent
#endif

GLBatch squareBatch;
GLShaderManager shaderManager;

GLfloat blockSize = 0.1f;

GLfloat vVerts[] = {
	-blockSize, -blockSize, 0.0f,
	 blockSize, -blockSize, 0.0f,
	 blockSize,  blockSize, 0.0f,
	-blockSize,  blockSize, 0.0f
};

// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}

///////////////////////////////////////////////////////////////////////////////

// This function does any needed initialization on the rendering context.
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC()
{
	// Blue background
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	shaderManager.InitializeStockShaders();

	squareBatch.Begin(GL_TRIANGLE_FAN, 4);
	squareBatch.CopyVertexData3f(vVerts);
	squareBatch.End();
}

///////////////////////////////////////////////////////////////////////////////

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	GLfloat stepSize = 0.025f;
	GLfloat blockX = vVerts[0]; // Upper left X
	GLfloat blockY = vVerts[7]; // Upper left Y

	// 方向键可以同时按下
	if (key == GLUT_KEY_UP)
	{
		blockY += stepSize;
	}
	if (key == GLUT_KEY_DOWN)
	{
		blockY -= stepSize;
	}
	if (key == GLUT_KEY_LEFT)
	{
		blockX -= stepSize;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		blockX += stepSize;
	}

	// Collision detection
	if (blockX < -1.0f)
	{
		blockX = -1.0f;
	}
	if (blockX > (1.0f - blockSize * 2))
	{
		blockX = 1.0f - blockSize * 2;
	}
	if (blockY < (-1.0f + blockSize * 2))
	{
		blockY = -1.0f + blockSize * 2;
	}
	if (blockY > 1.0f)
	{
		blockY = 1.0f;
	}

	// Recalculate vertex positions
	vVerts[0] = blockX;
	vVerts[1] = blockY - blockSize * 2;
	vVerts[3] = blockX + blockSize * 2;
	vVerts[4] = blockY - blockSize * 2;
	vVerts[6] = blockX + blockSize * 2;
	vVerts[7] = blockY;
	vVerts[9] = blockX;
	vVerts[10] = blockY;

	squareBatch.CopyVertexData3f(vVerts);

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////

// Called to draw scene
void RenderScene()
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_IDENTITY, vRed);

	squareBatch.Draw();

	// Perform the buffer swap to display the back buffer
	glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////

// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Move Block with Arrow Keys");

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKeys);

	SetupRC();

	glutMainLoop();

	return 0;
}