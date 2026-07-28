#ifndef UNIGL_H
#define UNIGL_H

#ifdef USEGLES
#if defined(MCPE_IOS) || defined(__APPLE__)
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
#include <GLES/gl.h>
#include <GLES/egl.h>
#include <GLES/glext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat znear, GLfloat zfar);

#ifdef __cplusplus
}
#endif

#else

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif

void initGlFuncs();
extern void (*glDeleteBuffers)(GLsizei n, const GLuint* buffers);
extern void (*glGenBuffers)(GLsizei n, GLuint* buffers);
extern void (*glBindBuffer)(GLenum target, GLuint buffer);
extern void (*glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);

#ifdef __cplusplus
}
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

int glhUnProjectf(float winx, float winy, float winz, float* modelview, float* projection, int* viewport, float* objectCoordinate);

#ifdef __cplusplus
}
#endif

#endif
