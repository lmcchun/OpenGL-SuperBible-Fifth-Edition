// New and improved (performance) sphere world

#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include <StopWatch.h>

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLShaderManager		shaderManager;

GLMatrixStack		modelViewMatrix;	// 模型视图矩阵
GLMatrixStack		projectionMatrix;	// 投影矩阵
GLFrustum			viewFrustum;		// 视景体
GLGeometryTransform	transformPipeline;	// 几何变换管线

GLTriangleBatch		torusBatch;
GLBatch				floorBatch;
GLTriangleBatch		sphereBatch;

GLFrame				cameraFrame;

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

	// This make a sphere
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);

	floorBatch.Begin(GL_LINES, 324);
	for (GLfloat x = -20.0f; x <= 20.0f; x += 0.5f)
	{
		floorBatch.Vertex3f(x, -0.55f, 20.0f);
		floorBatch.Vertex3f(x, -0.55f, -20.0f);
		floorBatch.Vertex3f(20.0f, -0.55f, x);
		floorBatch.Vertex3f(-20.0f, -0.55f, x);
	}
	floorBatch.End();

	for (int i = 0; i < NUM_SPHERES; ++i)
	{
		GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
		GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
		spheres[i].SetOrigin(x, 0.0f, z);
	}
}


///////////////////////////////////////////////////
// Screen changes size or is initialized
void ChangeSize(int nWidth, int nHeight)
{
	glViewport(0, 0, nWidth, nHeight);

	// 创建投影矩阵, 并将它载入到投影矩阵堆栈中
	viewFrustum.SetPerspective(35.0f, (float)nWidth / (float)nHeight, 1.0f, 100.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

	// 设置变换管线以使用两个矩阵堆栈
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

// 移动照相机参考帧来对方向键作出响应
void SpecialKeys(int key, int x, int y)
{
	float linear = 0.1f;
	float angular = (float)m3dDegToRad(5.0f);

	if (key == GLUT_KEY_UP)
	{
		cameraFrame.MoveForward(linear);
	}
	if (key == GLUT_KEY_DOWN)
	{
		cameraFrame.MoveForward(-linear);
	}
	if (key == GLUT_KEY_LEFT)
	{
		cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
	}
	if (key == GLUT_KEY_RIGHT)
	{
		cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
	}
}

// 进行调用以绘制场景
void RenderScene(void)
{
	// 颜色值
	static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat vSphereColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// 基于时间的动画
	static CStopWatch rotTimer;
	float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

	// 清除颜色缓冲区和深度缓冲区
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 保存当前的模型视图矩阵(单位矩阵)
	modelViewMatrix.PushMatrix();

	M3DMatrix44f mCamera;
	cameraFrame.GetCameraMatrix(mCamera);
	modelViewMatrix.PushMatrix(mCamera);

	// 绘制背景
	shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vFloorColor);
	floorBatch.Draw();

	for (int i = 0; i < NUM_SPHERES; ++i)
	{
		modelViewMatrix.PushMatrix();
		modelViewMatrix.MultMatrix(spheres[i]);
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vSphereColor);
		sphereBatch.Draw();
		modelViewMatrix.PopMatrix();
	}

	modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);

	// 保存平移
	modelViewMatrix.PushMatrix();

	// 应用旋转并绘制花托
	modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
	shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vTorusColor);
	torusBatch.Draw();

	// "清除" 以前的旋转
	modelViewMatrix.PopMatrix();

	// 应用另一个旋转, 然后进行平移, 然后在绘制球体
	modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
	modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
	shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vSphereColor);
	sphereBatch.Draw();

	// 保存以前的模型视图矩阵(单位矩阵)
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();

	// 进行缓冲区交换
	glutSwapBuffers();

	// 通知 GLUT 再进行一次同样操作
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800,600);

	glutCreateWindow("OpenGL SphereWorld");

	glutSpecialFunc(SpecialKeys);
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