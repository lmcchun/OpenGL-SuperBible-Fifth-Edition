#pragma once

#include "GLTools.h" // OpenGL toolkit

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include "gl/glut.h"
#endif

bool LoadTGATexture(const char* szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
bool LoadTGATextureRect(const char* szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);