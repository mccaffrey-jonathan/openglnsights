#ifndef COMMON_GL_H
#define COMMON_GL_H

#include "common/test.h"
#include "platform/gl.h"

typedef struct {
    GLuint vs;
    GLuint fs;
    GLuint prg;
} ShaderPair;

typedef struct {
    GLuint vbo;
    GLuint ibo;
    int indexCnt;
} Geometry;

TestError checkAndReportFramebufferStatus();
TestError checkAndReportCompilationStatus(GLuint name);
TestError checkAndReportLinkStatus(GLuint name);
TestError GenAndBindBuffers(Geometry* geo) ;
TestError BindBuffers(Geometry* geo) ;
void DrawGeo(Geometry* buffs);
TestError CompileShaders(ShaderPair* priv,
        const GLchar* vtxSrc,
        int vtxLen,
        const GLchar* frgSrc,
        int frgLen);
TestError DeleteGeometry(Geometry* geo) ;
TestError DeleteShaderPair(ShaderPair* shads) ;

#endif
