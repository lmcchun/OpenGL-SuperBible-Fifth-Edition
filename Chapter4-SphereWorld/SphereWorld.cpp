// New and improved (performance) sphere world

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLMatrixStack		modelViewMatrix;	// ģ����ͼ����
GLMatrixStack		projectionMatrix;	// ͶӰ����
GLFrustum			viewFrustum;		// �Ӿ���
GLGeometryTransform	transformPipeline;	// ���α任����
GLShaderManager		shaderManager;
GLTriangleBatch		torusBatch;
GLBatch				floorBatch;

//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context.
void SetupRC()
{
	// Initialze Shader Manager
	shaderManager.InitializeStockShaders();

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// This makes a torus
	gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);

	floorBatch.Begin(GL_LINES, 324);
	for (GLfloat x = -20.0f; x <= 20.0f; x += 0.5f)
	{
		floorBatch.Vertex3f(x, -0.55f, 20.0f);
		floorBatch.Vertex3f(x, -0.55f, -20.0f);
		floorBatch.Vertex3f(20.0f, -0.55f, x);
		floorBatch.Vertex3f(-20.0f, -0.55f, x);
	}
	floorBatch.End();    
}


///////////////////////////////////////////////////
// Screen changes size or is initialized
void ChangeSize(int nWidth, int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);

	// ����ͶӰ����, ���������뵽ͶӰ�����ջ��
	viewFrustum.SetPerspective(35.0f, (float)nWidth / (float)nHeight, 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	// ���ñ任������ʹ�����������ջ
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

// ���е����Ի��Ƴ���
void RenderScene(void)
{
	// ��ɫֵ
	static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	// ����ʱ��Ķ���
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	// �����ɫ����������Ȼ�����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ���浱ǰ��ģ����ͼ����(��λ����)
	modelViewMatrix.PushMatrix();

	// ���Ʊ���
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vFloorColor);
	floorBatch.Draw();

	// ������ת�Ļ���
	modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);
	modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
	shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vTorusColor);
	torusBatch.Draw();

	// ������ǰ��ģ����ͼ����(��λ����)
	modelViewMatrix.PopMatrix();

	// ���л���������
	glutSwapBuffers();

	// ֪ͨ GLUT �ٽ���һ��ͬ������
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800,600);

	glutCreateWindow("OpenGL SphereWorld");

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
	return 0;
}