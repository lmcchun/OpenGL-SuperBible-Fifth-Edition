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

GLfloat xPos = 0.0f;
GLfloat yPos = 0.0;

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

	// 方向键可以同时按下
	if (key == GLUT_KEY_UP)
	{
		yPos += stepSize;
	}
	if (key == GLUT_KEY_DOWN)
	{
		yPos -= stepSize;
	}
	if (key == GLUT_KEY_LEFT)
	{
		xPos -= stepSize;
	}
	if (key == GLUT_KEY_RIGHT)
	{
		xPos += stepSize;
	}

	// Collision detection
	if (xPos < -1.0f)
	{
		xPos = -1.0f + blockSize;
	}
	if (xPos > (1.0f - blockSize))
	{
		xPos = 1.0f - blockSize;
	}
	if (yPos < (-1.0f + blockSize))
	{
		yPos = -1.0f + blockSize;
	}
	if (yPos > (1.0f - blockSize))
	{
		yPos = 1.0f - blockSize;
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////

// Called to draw scene
void RenderScene()
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	M3DMatrix44f mFinalTransform;
	M3DMatrix44f mTranslationMatrix;
	M3DMatrix44f mRotationMatrix;

	// Just Translate
	m3dTranslationMatrix44(mTranslationMatrix, xPos, yPos, 0.0f);

	// Rotate 5 degrees everytime we redraw
	static float zRot = 0.0f;
	zRot += 5.0f;
	m3dRotationMatrix44(mRotationMatrix, (float)m3dDegToRad(zRot), 0.0f, 0.0f, 1.0f);

	m3dMatrixMultiply44(mFinalTransform, mTranslationMatrix, mRotationMatrix);

	shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, mFinalTransform, vRed);

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
	glutInitWindowSize(600, 600);
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