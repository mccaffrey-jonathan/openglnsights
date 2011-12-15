#ifndef COMMON_SCENE_SETUP_H
#define COMMON_SCENE_SETUP_H

#include "platform/gl.h"
#include "common/gl.h"

//#define WIDTH 1024
//#define HEIGHT 1024
#define WIDTH 1024
#define HEIGHT 1024
//#define SCENE_VERTEX_WIDTH 160
//#define SCENE_VERTEX_HEIGHT 160
#define SCENE_VERTEX_WIDTH 140
#define SCENE_VERTEX_HEIGHT 140

#define BYTES_PER_PIXEL 2
#define WARMUP_ITERS 10
//#define TEST_ITERS (1024LL) / BYTES_PER_PIXEL
#define TEST_ITERS 500

typedef struct {
    GLint aPosition;
    GLint aNormal;
    GLint textureCoord;
} SceneAttribLocs;

typedef struct {
    GLint uMVPMatrix;
    GLint lightPos;
    GLint matDiffuse;
    GLint texture1;
} SceneUniformLocs;

typedef struct {
    GLuint fbo;
    GLuint depth;
    GLuint color;
} SceneFramebuffer;

typedef struct {
    float pos[3];
    float nrm[3];
    float tex[2];
} SceneVertex;

TestError SetupDepthAndColorFbo(SceneFramebuffer* fb);
TestError SetupDepthRBOAndColorTex(SceneFramebuffer* fb);
TestError bufferDataSceneVertexGrid(int width, int height) ;
TestError CompileSceneShaders(ShaderPair* priv);
TestError CompileSceneShadersFragVersion(ShaderPair* priv);
TestError GetSceneUniformAndAttribLocs(GLuint prg,
        SceneAttribLocs* attribs,
        SceneUniformLocs* uniforms);
TestError GenAndBindBuffers(Geometry* geo) ;
TestError CreateTexture(GLuint* out) ;

TestError BindSceneShader(
        GLuint prg,
        GLuint tex,
        SceneAttribLocs* attribs,
        SceneUniformLocs* uniforms);
void DrawScene(Geometry* buffs);

TestError DeleteSceneFramebufferWithTex(SceneFramebuffer* fb) ;

TestError DeleteSceneFramebuffer(SceneFramebuffer* fb) ;

void DebugDumpPixels();
void DebugDumpVertices();
void DebugDumpIndices();
void DebugDumpAttribsAndUniforms(GLuint prg);
void DebugDumpTexture(GLuint tex, int width, int height);

#endif
