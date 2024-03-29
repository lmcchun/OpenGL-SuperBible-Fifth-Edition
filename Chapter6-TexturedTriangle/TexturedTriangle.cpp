#include "Common.h"

#include "GLShaderManager.h" // Shader Manager Class


GLBatch triangleBatch;
GLShaderManager shaderManager;

GLint myTexturedIdentityShader;
GLuint textureID;

// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}

// This function does any needed initialization on the rendering context
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	shaderManager.InitializeStockShaders();

	// Load up a triangle
	GLfloat vVerts[] = {
		-0.5f, 0.0f, 0.0f,
		 0.5f, 0.0f, 0.0f,
		 0.0f, 0.5f, 0.0f
	};

	GLfloat vTexCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};

	triangleBatch.Begin(GL_TRIANGLES, 3, 1);
	triangleBatch.CopyVertexData3f(vVerts);
	triangleBatch.CopyTexCoordData2f(vTexCoords, 0);
	triangleBatch.End();

	const char* szVertexProgFileName = "../Res/Chapter6-TexturedTriangle/TexturedTriangle.vp";
	const char* szFragmentProgFileName = "../Res/Chapter6-TexturedTriangle/TexturedTriangle.fp";
	myTexturedIdentityShader = shaderManager.LoadShaderPairWithAttributes(szVertexProgFileName, szFragmentProgFileName, 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	LoadTGATexture("../Res/Chapter6-TexturedTriangle/stone.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
}

// Cleanup
void ShutdownRC()
{
	glDeleteProgram(myTexturedIdentityShader);
	glDeleteTextures(1, &textureID);
}

// Called to draw scene
void RenderScene(void)
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(myTexturedIdentityShader);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLint iTextureUniform = glGetUniformLocation(myTexturedIdentityShader, "colorMap");
	glUniform1i(iTextureUniform, 0); // XXX ������Ԫ 0

	triangleBatch.Draw();

	// Perform the buffer swap to display back buffer
	glutSwapBuffers();
}

// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Textured Triangle");
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