#include <GLTools.h> // OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLGeometryTransform.h>

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

/////////////////////////////////////////////////////////////////////////////////
// An assortment of needed classes
GLShaderManager		shaderManager;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
GLFrame				cameraFrame;
GLFrame             objectFrame;
GLFrustum			viewFrustum;

GLBatch				pointBatch;
GLBatch				lineBatch;
GLBatch				lineStripBatch;
GLBatch				lineLoopBatch;
GLBatch				triangleBatch;
GLBatch             triangleStripBatch;
GLBatch             triangleFanBatch;

GLGeometryTransform	transformPipeline;
M3DMatrix44f		shadowMatrix;


GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };


// Keep track of effects step
int nStep = 0;


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context. 
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC()
{
	// Black background
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f );

	shaderManager.InitializeStockShaders();

	glEnable(GL_DEPTH_TEST);

	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

	cameraFrame.MoveForward(-15.0f);

	//////////////////////////////////////////////////////////////////////
	// Some points, more or less in the shape of Florida
	GLfloat vCoast[24][3] = {
		{  2.80f,  1.20f, 0.00f }, {  2.00f,  1.20f, 0.00f },
		{  2.00f,  1.08f, 0.00f }, {  2.00f,  1.08f, 0.00f },
		{  0.00f,  0.80f, 0.00f }, { -0.32f,  0.40f, 0.00f },
		{ -0.48f,  0.20f, 0.00f }, { -0.40f,  0.00f, 0.00f },
		{ -0.60f, -0.40f, 0.00f }, { -0.80f, -0.80f, 0.00f },
		{ -0.80f, -1.40f, 0.00f }, { -0.40f, -1.60f, 0.00f },
		{  0.00f, -1.20f, 0.00f }, {  0.20f, -0.80f, 0.00f },
		{  0.48f, -0.40f, 0.00f }, {  0.52f, -0.20f, 0.00f },
		{  0.48f,  0.20f, 0.00f }, {  0.80f,  0.40f, 0.00f },
		{  1.20f,  0.80f, 0.00f }, {  1.60f,  0.60f, 0.00f },
		{  2.00f,  0.60f, 0.00f }, {  2.20f,  0.80f, 0.00f },
		{  2.40f,  1.00f, 0.00f }, {  2.80f,  1.00f, 0.00f }
	};

	// Load point batch
	pointBatch.Begin(GL_POINTS, 24);
	pointBatch.CopyVertexData3f(vCoast);
	pointBatch.End();

	// Load as a bunch of line segments
	lineBatch.Begin(GL_LINES, 24);
	lineBatch.CopyVertexData3f(vCoast);
	lineBatch.End();

	// Load as a single line segment
	lineStripBatch.Begin(GL_LINE_STRIP, 24);
	lineStripBatch.CopyVertexData3f(vCoast);
	lineStripBatch.End();

	// Single line, connect first and last points
	lineLoopBatch.Begin(GL_LINE_LOOP, 24);
	lineLoopBatch.CopyVertexData3f(vCoast);
	lineLoopBatch.End();

	// For Triangles, we'll make a Pyramid
	GLfloat vPyramid[12][3] = {
		-2.0f, 0.0f, -2.0f, 
		 2.0f, 0.0f, -2.0f, 
		 0.0f, 4.0f,  0.0f,

		2.0f, 0.0f, -2.0f,
		2.0f, 0.0f,  2.0f,
		0.0f, 4.0f,  0.0f,

		 2.0f, 0.0f, 2.0f,
		-2.0f, 0.0f, 2.0f,
		 0.0f, 4.0f, 0.0f,

		-2.0f, 0.0f,  2.0f,
		-2.0f, 0.0f, -2.0f,
		 0.0f, 4.0f,  0.0f
	};

	triangleBatch.Begin(GL_TRIANGLES, 12);
	triangleBatch.CopyVertexData3f(vPyramid);
	triangleBatch.End();


	// For a Triangle fan, just a 6 sided hex. Raise the center up a bit
	GLfloat vPoints[100][3]; // Scratch array, more than we need
	int nVerts = 0;
	GLfloat r = 3.0f;
	vPoints[nVerts][0] = 0.0f;
	vPoints[nVerts][1] = 0.0f;
	vPoints[nVerts][2] = 0.0f;

	GLfloat fAngleStep = static_cast<GLfloat>(M3D_2PI / 6.0f);
	for (GLfloat angle = 0; angle < M3D_2PI; angle += fAngleStep)
	{
		nVerts++;
		vPoints[nVerts][0] = static_cast<float>(cos(angle)) * r;
		vPoints[nVerts][1] = static_cast<float>(sin(angle)) * r;
		vPoints[nVerts][2] = -0.5f;
	}

	// Close the fan
	nVerts++;
	vPoints[nVerts][0] = r;
	vPoints[nVerts][1] = 0;
	vPoints[nVerts][2] = 0.0f;

	// Load it up
	triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
	triangleFanBatch.CopyVertexData3f(vPoints);
	triangleFanBatch.End();     

	// For triangle strips, a little ring or cylinder segment
	int iCounter = 0;
	GLfloat radius = 3.0f;
	for (GLfloat angle = 0.0f; angle <= (2.0f * M3D_PI); angle += 0.3f)
	{
		GLfloat x = radius * sin(angle);
		GLfloat y = radius * cos(angle);

		// Specify the point and move the Z value up a little	
		vPoints[iCounter][0] = x;
		vPoints[iCounter][1] = y;
		vPoints[iCounter][2] = -0.5;
		iCounter++;

		vPoints[iCounter][0] = x;
		vPoints[iCounter][1] = y;
		vPoints[iCounter][2] = 0.5;
		iCounter++;            
	}

	// Close up the loop
	vPoints[iCounter][0] = vPoints[0][0];
	vPoints[iCounter][1] = vPoints[0][1];
	vPoints[iCounter][2] = -0.5;
	iCounter++;

	vPoints[iCounter][0] = vPoints[1][0];
	vPoints[iCounter][1] = vPoints[1][1];
	vPoints[iCounter][2] = 0.5;
	iCounter++;            

	// Load the triangle strip
	triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
	triangleStripBatch.CopyVertexData3f(vPoints);
	triangleStripBatch.End();    
}


/////////////////////////////////////////////////////////////////////////
void DrawWireFramedBatch(GLBatch* pBatch)
{
	// Draw the batch solid green
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
	pBatch->Draw();

	// Draw black outline
	glPolygonOffset(-1.0f, -1.0f);      // Shift depth values
	glEnable(GL_POLYGON_OFFSET_LINE);

	// Draw lines antialiased
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw black wireframe version of geometry
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.5f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
	pBatch->Draw();

	// Put everything back the way we found it
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glLineWidth(1.0f);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
}


///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
{    
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	modelViewMatrix.PushMatrix();
	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.MultMatrix(mCamera);

	M3DMatrix44f mObjectFrame;
	objectFrame.GetMatrix(mObjectFrame);
	modelViewMatrix.MultMatrix(mObjectFrame);

	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);

	switch (nStep)
	{
	case 0:
		glPointSize(4.0f);
		pointBatch.Draw();
		glPointSize(1.0f);
		break;
	case 1:
		glLineWidth(2.0f);
		lineBatch.Draw();
		glLineWidth(1.0f);
		break;
	case 2:
		glLineWidth(2.0f);
		lineStripBatch.Draw();
		glLineWidth(1.0f);
		break;
	case 3:
		glLineWidth(2.0f);
		lineLoopBatch.Draw();
		glLineWidth(1.0f);
		break;
	case 4:
		DrawWireFramedBatch(&triangleBatch);
		break;
	case 5:
		DrawWireFramedBatch(&triangleStripBatch);
		break;
	case 6:
		DrawWireFramedBatch(&triangleFanBatch);
		break;
	}

	modelViewMatrix.PopMatrix();

	// Flush drawing commands
	glutSwapBuffers();
}


// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		objectFrame.RotateWorld(static_cast<float>(m3dDegToRad(-5.0f)), 1.0f, 0.0f, 0.0f);
	}

	if (key == GLUT_KEY_DOWN)
	{
		objectFrame.RotateWorld(static_cast<float>(m3dDegToRad(5.0f)), 1.0f, 0.0f, 0.0f);
	}

	if (key == GLUT_KEY_LEFT)
	{
		objectFrame.RotateWorld(static_cast<float>(m3dDegToRad(-5.0f)), 0.0f, 1.0f, 0.0f);
	}

	if (key == GLUT_KEY_RIGHT)
	{
		objectFrame.RotateWorld(static_cast<float>(m3dDegToRad(5.0f)), 0.0f, 1.0f, 0.0f);
	}

	glutPostRedisplay();
}




///////////////////////////////////////////////////////////////////////////////
// A normal ASCII key has been pressed.
// In this case, advance the scene when the space bar is pressed
void KeyPressFunc(unsigned char key, int x, int y)
{
	if (key == 32)
	{
		nStep++;

		if (nStep > 6)
		{
			nStep = 0;
		}
	}

	switch(nStep)
	{
	case 0: 
		glutSetWindowTitle("GL_POINTS");
		break;
	case 1:
		glutSetWindowTitle("GL_LINES");
		break;
	case 2:
		glutSetWindowTitle("GL_LINE_STRIP");
		break;
	case 3:
		glutSetWindowTitle("GL_LINE_LOOP");
		break;
	case 4:
		glutSetWindowTitle("GL_TRIANGLES");
		break;
	case 5:
		glutSetWindowTitle("GL_TRIANGLE_STRIP");
		break;
	case 6:
		glutSetWindowTitle("GL_TRIANGLE_FAN");
		break;
	}

	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	modelViewMatrix.LoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("GL_POINTS");
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyPressFunc);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	SetupRC();

	glutMainLoop();

	return 0;
}
