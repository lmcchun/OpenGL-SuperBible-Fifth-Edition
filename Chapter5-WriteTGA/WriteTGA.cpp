#include "GLTools.h" // OpenGL toolkit
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////
// Define targa header. This is only used locally.
#pragma pack(1)
typedef struct
{
    GLbyte	identsize;              // Size of ID field that follows header (0)
    GLbyte	colorMapType;           // 0 = None, 1 = paletted
    GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
    unsigned short	colorMapStart;          // First colour map entry
    unsigned short	colorMapLength;         // Number of colors
    unsigned char 	colorMapBits;   // bits per palette entry
    unsigned short	xstart;                 // image x origin
    unsigned short	ystart;                 // image y origin
    unsigned short	width;                  // width in pixels
    unsigned short	height;                 // height in pixels
    GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
    GLbyte	descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)

/////////////////////////////////////////////////////////////////////////////////
// An assortment of needed classes
GLShaderManager		shaderManager;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
GLFrame				cameraFrame;
GLFrustum			viewFrustum;
GLBatch				cubeBatch;
GLBatch				floorBatch;
GLBatch				topBlock;
GLBatch				frontBlock;
GLBatch				leftBlock;

GLGeometryTransform	transformPipeline;
M3DMatrix44f		shadowMatrix;


// Keep track of effects step
int nStep = 0;

// Lighting data
GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat lightSpecular[] = { 0.9f, 0.9f, 0.9f };
GLfloat vLightPos[] = { -8.0f, 20.0f, 100.0f, 1.0f };

GLuint textures[4];


///////////////////////////////////////////////////////////////////////////////
// Make a cube out of a batch of triangles. Texture coordinates and normals
// are also provided.
void MakeCube(GLBatch& cubeBatch)
{
	cubeBatch.Begin(GL_TRIANGLES, 36, 1);

	/////////////////////////////////////////////
	// Top of cube
	cubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, 1.0f);


	////////////////////////////////////////////
	// Bottom of cube
	cubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, 1.0f);

	///////////////////////////////////////////
	// Left side of cube
	cubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, 1.0f);

	// Right side of cube
	cubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, 1.0f);

	cubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, -1.0f);

	// Front and Back
	// Front
	cubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, 1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, 1.0f);

	// Back
	cubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	cubeBatch.Vertex3f(-1.0f, -1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
	cubeBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	cubeBatch.Vertex3f(-1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	cubeBatch.Vertex3f(1.0f, 1.0f, -1.0f);

	cubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
	cubeBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	cubeBatch.Vertex3f(1.0f, -1.0f, -1.0f);

	cubeBatch.End();
}

/////////////////////////////////////////////////////////////////////////////
// Make the floor, just the verts and texture coordinates, no normals
void MakeFloor(GLBatch& floorBatch)
{
	GLfloat x = 5.0f;
	GLfloat y = -1.0f;

	floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
	floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
	floorBatch.Vertex3f(-x, y, x);

	floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
	floorBatch.Vertex3f(x, y, x);

	floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
	floorBatch.Vertex3f(x, y, -x);

	floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
	floorBatch.Vertex3f(-x, y, -x);
	floorBatch.End();
}


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context. 
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC()
{
	GLbyte *pBytes;
	GLint nWidth, nHeight, nComponents;
	GLenum format;

	shaderManager.InitializeStockShaders();

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.5f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

	cameraFrame.MoveForward(-15.0f);
	cameraFrame.MoveUp(6.0f);
	cameraFrame.RotateLocalX((float)m3dDegToRad(20.0f));

	MakeCube(cubeBatch);
	MakeFloor(floorBatch);

	// Make top
	topBlock.Begin(GL_TRIANGLE_FAN, 4, 1);
	topBlock.Normal3f(0.0f, 1.0f, 0.0f);
	topBlock.MultiTexCoord2f(0, 0.0f, 0.0f);
	topBlock.Vertex3f(-1.0f, 1.0f, 1.0f);

	topBlock.Normal3f(0.0f, 1.0f, 0.0f);
	topBlock.MultiTexCoord2f(0, 1.0f, 0.0f);
	topBlock.Vertex3f(1.0f, 1.0f, 1.0f);

	topBlock.Normal3f(0.0f, 1.0f, 0.0f);
	topBlock.MultiTexCoord2f(0, 1.0f, 1.0f);
	topBlock.Vertex3f(1.0f, 1.0f, -1.0f);

	topBlock.Normal3f(0.0f, 1.0f, 0.0f);
	topBlock.MultiTexCoord2f(0, 0.0f, 1.0f);
	topBlock.Vertex3f(-1.0f, 1.0f, -1.0f);
	topBlock.End();

	// Make Front
	frontBlock.Begin(GL_TRIANGLE_FAN, 4, 1);
	frontBlock.Normal3f(0.0f, 0.0f, 1.0f);
	frontBlock.MultiTexCoord2f(0, 0.0f, 0.0f);
	frontBlock.Vertex3f(-1.0f, -1.0f, 1.0f);

	frontBlock.Normal3f(0.0f, 0.0f, 1.0f);
	frontBlock.MultiTexCoord2f(0, 1.0f, 0.0f);
	frontBlock.Vertex3f(1.0f, -1.0f, 1.0f);

	frontBlock.Normal3f(0.0f, 0.0f, 1.0f);
	frontBlock.MultiTexCoord2f(0, 1.0f, 1.0f);
	frontBlock.Vertex3f(1.0f, 1.0f, 1.0f);

	frontBlock.Normal3f(0.0f, 0.0f, 1.0f);
	frontBlock.MultiTexCoord2f(0, 0.0f, 1.0f);
	frontBlock.Vertex3f(-1.0f, 1.0f, 1.0f);
	frontBlock.End();

	// Make left
	leftBlock.Begin(GL_TRIANGLE_FAN, 4, 1);
	leftBlock.Normal3f(-1.0f, 0.0f, 0.0f);
	leftBlock.MultiTexCoord2f(0, 0.0f, 0.0f);
	leftBlock.Vertex3f(-1.0f, -1.0f, -1.0f);

	leftBlock.Normal3f(-1.0f, 0.0f, 0.0f);
	leftBlock.MultiTexCoord2f(0, 1.0f, 0.0f);
	leftBlock.Vertex3f(-1.0f, -1.0f, 1.0f);

	leftBlock.Normal3f(-1.0f, 0.0f, 0.0f);
	leftBlock.MultiTexCoord2f(0, 1.0f, 1.0f);
	leftBlock.Vertex3f(-1.0f, 1.0f, 1.0f);

	leftBlock.Normal3f(-1.0f, 0.0f, 0.0f);
	leftBlock.MultiTexCoord2f(0, 0.0f, 1.0f);
	leftBlock.Vertex3f(-1.0f, 1.0f, -1.0f);
	leftBlock.End();

	// Create shadow projection matrix
	GLfloat floorPlane[] = { 0.0f, 1.0f, 0.0f, 1.0f};
	m3dMakePlanarShadowMatrix(shadowMatrix, floorPlane, vLightPos);

	// Load up four textures  
	glGenTextures(4, textures);

	// Wood floor
	pBytes = gltReadTGABits("../Res/Block/floor.tga", &nWidth, &nHeight, &nComponents, &format);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0, format, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	// One of the block faces
	pBytes = gltReadTGABits("../Res/Block/Block4.tga", &nWidth, &nHeight, &nComponents, &format);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0, format, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	// Another block face
	pBytes = gltReadTGABits("../Res/Block/Block5.tga", &nWidth, &nHeight, &nComponents, &format);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0, format, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);

	// Yet another block face
	pBytes = gltReadTGABits("../Res/Block/Block6.tga", &nWidth, &nHeight, &nComponents, &format);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,nComponents,nWidth, nHeight, 0, format, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
}

///////////////////////////////////////////////////////////////////////////////
// Render the block
void RenderBlock(void)
{
	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	switch(nStep)
	{
	case 0: // Wire frame
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);

		// Draw the cube
		cubeBatch.Draw();

		break;

	case 1: // Wire frame, but not the back side... we also want the block to be in the stencil buffer
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);

		// Draw solid block in stencil buffer
		// Back face culling prevents the back sides from showing through
		// The stencil pattern is used to mask when we draw the floor under it
		// to keep it from showing through.
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NEVER, 0, 0);
		glStencilOp(GL_INCR, GL_INCR, GL_INCR);
		cubeBatch.Draw();
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDisable(GL_STENCIL_TEST);

		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Draw the front side cube
		cubeBatch.Draw();
		break;

	case 2: // Solid
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);

		// Draw the cube
		cubeBatch.Draw();
		break;

	case 3: // Solit & Lit
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_POINT_LIGHT_DIFF, modelViewMatrix.GetMatrix(), projectionMatrix.GetMatrix(), vLightPos, vRed);

		// Draw the cube
		cubeBatch.Draw();
		break;

	case 4: // Textured
	case 5: // Textured & Lit
	default:
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, modelViewMatrix.GetMatrix(), projectionMatrix.GetMatrix(), vLightPos, vWhite, 0);

		glBindTexture(GL_TEXTURE_2D, textures[1]);
		topBlock.Draw();
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		frontBlock.Draw();
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		leftBlock.Draw();

		break;
	}

	// Put everything back
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_STENCIL_TEST);
}

///////////////////////////////////////////////////////////////////////////////
// Render the floor
void RenderFloor(void)
{
	GLfloat vBrown [] = { 0.55f, 0.292f, 0.09f, 1.0f};
	GLfloat vFloor[] = { 1.0f, 1.0f, 1.0f, 0.6f };

	switch(nStep)
	{
	case 0: // Wire frame
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBrown);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		break;

	case 1: // Wire frame, but not the back side.. and only where stencil == 0
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0, 0xff);

		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBrown);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;

	case 2: // Solid
	case 3:	// Solit & Lit
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBrown);
		break;

	case 4: // Textured
	case 5: // Textured & Lit
	default:
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		shaderManager.UseStockShader(GLT_STOCK_SHADER::GLT_SHADER_TEXTURE_MODULATE, transformPipeline.GetModelViewProjectionMatrix(), vFloor, 0);
		break;
	}

	// Draw the floor
	floorBatch.Draw();

	// Put everything back
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_STENCIL_TEST);
}

///////////////////////////////////////////////////////////////////////////////
// 捕获当前视口并将它保存为一个 Targa 文件.
// 确保在对调用这个函数之前, 在双缓冲区环境下调用 SwapBuffers, 而在单缓冲区环境下调用 glFinish.
// 如果出现错误则返回 0, 如果没有出现错误则返回 1
GLint gltWriteTGA(const char *szFileName)
{
	FILE			*pFile;			// 文件指针
	TGAHEADER		tgaHeader;		// TGA 文件头
	unsigned long	ulImageSize;	// 图像的大小, 用字节表示
	GLbyte			*pBits = NULL;	// 指向位的指针
	GLint			iViewport[4];	// 以像素表示的视口
	GLenum			lastBuffer;		// 存储当前的读取缓冲区设置

	// 获取视口大小
	glGetIntegerv(GL_VIEWPORT, iViewport);

	// 图像应该多大(Targa 文件将被紧密包装)
	ulImageSize = iViewport[2] * 3 * iViewport[3];

	// 分配块, 如果这种操作不起作用则返回
	pBits = (GLbyte *)malloc(ulImageSize);
	if (!pBits)
	{
		return 0;
	}

	// 从颜色缓冲区读取位
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	// 获取当前读取缓冲设置并进行保存.
	// 切换到前台缓冲区并进行读取操作, 最后恢复读取缓冲区状态.
	glGetIntegerv(GL_READ_BUFFER, (GLint *)(&lastBuffer));
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR, GL_UNSIGNED_BYTE, pBits);
	glReadBuffer(lastBuffer);

	// 初始化 Targa 头
	tgaHeader.identsize = 0;
	tgaHeader.colorMapType = 0;
	tgaHeader.imageType = 2;
	tgaHeader.colorMapStart = 0;
	tgaHeader.colorMapLength = 0;
	tgaHeader.colorMapBits = 0;
	tgaHeader.xstart = 0;
	tgaHeader.ystart = 0;
	tgaHeader.width = iViewport[2];
	tgaHeader.height = iViewport[3];
	tgaHeader.bits = 24;
	tgaHeader.descriptor = 0;

	// 为大小字节存储顺序问题而进行字节交换
#ifdef __APPLE__
	LITTLE_ENDIAN_WORD(&tgaHeader.colorMapStart);
	LITTLE_ENDIAN_WORD(&tgaHeader.colorMapLength);
	LITTLE_ENDIAN_WORD(&tgaHeader.xstart);
	LITTLE_ENDIAN_WORD(&tgaHeader.ystart);
	LITTLE_ENDIAN_WORD(&tgaHeader.width);
	LITTLE_ENDIAN_WORD(&tgaHeader.height);
#endif

	// 尝试打开文件
	errno_t err = fopen_s(&pFile, szFileName, "wb");
	if (err || !pFile)
	{
		free(pBits); // 释放缓冲区并返回错误
		return 0;
	}

	// 写入文件头
	fwrite(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	// 写入图像数据
	fwrite(pBits, ulImageSize, 1, pFile);

	// 释放临时缓冲区并关闭文件
	free(pBits);
	fclose(pFile);

	// 成功了!
	return 1;
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

	// Reflection step... draw cube upside down, the floor
	// blended on top of it
	if (nStep == 5)
	{
		glDisable(GL_CULL_FACE);
		modelViewMatrix.PushMatrix();
		modelViewMatrix.Scale(1.0f, -1.0f, 1.0f);
		modelViewMatrix.Translate(0.0f, 2.0f, 0.0f);
		modelViewMatrix.Rotate(35.0f, 0.0f, 1.0f, 0.0f);
		RenderBlock();
		modelViewMatrix.PopMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		RenderFloor();
		glDisable(GL_BLEND);			
	}

	modelViewMatrix.PushMatrix();

	// Draw normally
	modelViewMatrix.Rotate(35.0f, 0.0f, 1.0f, 0.0f);
	RenderBlock();
	modelViewMatrix.PopMatrix();

	// If not the reflection pass, draw floor last
	if (nStep != 5)
	{
		RenderFloor();
	}

	modelViewMatrix.PopMatrix();

	// Flush drawing commands
	glutSwapBuffers();

	gltWriteTGA("block.tga");
}


///////////////////////////////////////////////////////////////////////////////
// A normal ASCII key has been pressed.
// In this case, advance the scene when the space bar is pressed
void KeyPressFunc(unsigned char key, int x, int y)
{
	if (key == 32)
	{
		nStep++;

		if (nStep > 5)
		{
			nStep = 0;
		}
	}

	// Refresh the Window
	glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	viewFrustum.SetPerspective(35.0f, static_cast<float>(w) / static_cast<float>(h), 1.0f, 500.0f);
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
	glutCreateWindow("Write TGA Demo");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyPressFunc);
	glutDisplayFunc(RenderScene);

	SetupRC();

	glutMainLoop();
	glDeleteTextures(4,textures);
	return 0;
}
