// ��ʾ Mip ��ͼ�Լ�ʹ���������
#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLFrame.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager		shaderManager;		// ��ɫ��������
GLMatrixStack		modelViewMatrix;	// ģ����ͼ����
GLMatrixStack		projectionMatrix;	// ͶӰ����
GLFrustum			viewFrustum;		// �Ӿ���
GLGeometryTransform	transformPipeline;	// ���α任����

GLBatch             floorBatch;
GLBatch             ceilingBatch;
GLBatch             leftWallBatch;
GLBatch             rightWallBatch;

GLfloat             viewZ = -65.0f;

// �������
#define TEXTURE_BRICK   0
#define TEXTURE_FLOOR   1
#define TEXTURE_CEILING 2
#define TEXTURE_COUNT   3
GLuint  textures[TEXTURE_COUNT];
const char *szTextureFiles[TEXTURE_COUNT] = {
	"../Res/Tunnel/brick.tga",
	"../Res/Tunnel/floor.tga",
	"../Res/Tunnel/ceiling.tga"
};

///////////////////////////////////////////////////////////////////////////////
// Ϊÿ���������ı����������
void ProcessMenu(int value)
{
	GLint iLoop;

	for (iLoop = 0; iLoop < TEXTURE_COUNT; ++iLoop)
	{
		glBindTexture(GL_TEXTURE_2D, textures[iLoop]);

		switch(value)
		{
		case 0:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			break;

		case 1:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;

		case 2:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			break;

		case 3:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			break;

		case 4:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;

		case 5:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;                
		}
	}

	// �����ػ�
	glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////
// ��������ܹ�����Ⱦ�����н����κ���Ҫ���Ż�.
// �����������ò���ʼ���������.
void SetupRC()
{
	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;
	GLint iLoop;

	// ��ɫ����
	glClearColor(0.0f, 0.0f, 0.0f,1.0f);

	shaderManager.InitializeStockShaders();

	// ��������
	glGenTextures(TEXTURE_COUNT, textures);
	for (iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
	{
		// ����һ���������
		glBindTexture(GL_TEXTURE_2D, textures[iLoop]);

		// ��������
		pBytes = gltReadTGABits(szTextureFiles[iLoop], &iWidth, &iHeight, &iComponents, &eFormat);

		// ���ù������ͻ���ģʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(pBytes); // ������Ҫԭʼ����������
	}

	// ��������ͼ��
	GLfloat z;
	floorBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
	for (z = 60.0f; z >= 0.0f; z -=10.0f)
	{
		floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
		floorBatch.Vertex3f(-10.0f, -10.0f, z);

		floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
		floorBatch.Vertex3f(10.0f, -10.0f, z);

		floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
		floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);

		floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
		floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
	}
	floorBatch.End();

	ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
	for (z = 60.0f; z >= 0.0f; z -= 10.0f)
	{
		ceilingBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
		ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);

		ceilingBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
		ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);

		ceilingBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
		ceilingBatch.Vertex3f(-10.0f, 10.0f, z);

		ceilingBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
		ceilingBatch.Vertex3f(10.0f, 10.0f, z);
	}
	ceilingBatch.End();

	leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
	for (z = 60.0f; z >= 0.0f; z -= 10.0f)
	{
		leftWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
		leftWallBatch.Vertex3f(-10.0f, -10.0f, z);

		leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
		leftWallBatch.Vertex3f(-10.0f, 10.0f, z);

		leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
		leftWallBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);

		leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
		leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
	}
	leftWallBatch.End();


	rightWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
	for (z = 60.0f; z >= 0.0f; z -= 10.0f)
	{
		rightWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
		rightWallBatch.Vertex3f(10.0f, -10.0f, z);

		rightWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
		rightWallBatch.Vertex3f(10.0f, 10.0f, z);

		rightWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
		rightWallBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);

		rightWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
		rightWallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
	}
	rightWallBatch.End();
}

///////////////////////////////////////////////////
// �ر���Ⱦ����, ɾ��������󼴿�
void ShutdownRC(void)
{
	glDeleteTextures(TEXTURE_COUNT, textures);
}


///////////////////////////////////////////////////
// ǰ���ƶ��ӿ����Է����������Ӧ
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
	{
		viewZ += 0.5f;
	}

	if (key == GLUT_KEY_DOWN)
	{
		viewZ -= 0.5f;
	}

	// Refresh the Window
	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////
// �ı��Ӿ�����ӿ�, �ڸı䴰�ڴ�Сʱ����
void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// ��ֹ�� 0 ���г�������
	if (h == 0)
	{
		h = 1;
	}

	// ���ӿ�����Ϊ���ڴ�С
	glViewport(0, 0, w, h);

	fAspect = (GLfloat)w / (GLfloat)h;

	// ����͸��ͶӰ
	viewFrustum.SetPerspective(80.0f, fAspect, 1.0, 120.0);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

}

///////////////////////////////////////////////////////
// ���е����Ի��Ƴ���
void RenderScene(void)
{
	// �õ�ǰ���ɫ�������
	glClear(GL_COLOR_BUFFER_BIT);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, 0.0f, viewZ);

	shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);

	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
	floorBatch.Draw();

	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILING]);
	ceilingBatch.Draw();

	glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
	leftWallBatch.Draw();
	rightWallBatch.Draw();

	modelViewMatrix.PopMatrix();

	// ����������
	glutSwapBuffers();
}


//////////////////////////////////////////////////////
// ������ڵ�
int main(int argc, char *argv[])
{
	gltSetWorkingDirectory(argv[0]);

	// ��׼��ʼ��
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Tunnel");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	// ��Ӳ˵�����Ըı������
	glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("GL_NEAREST",0);
	glutAddMenuEntry("GL_LINEAR",1);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST",2);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 3);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 4);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	// ����, ѭ��, �ر�
	SetupRC();
	glutMainLoop();
	ShutdownRC();

	return 0;
}
