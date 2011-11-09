#define COMMON_POST_SETUP_H
#define COMMON_POST_SETUP_H

typedef struct {
    GLint aPosition;
    GLint textureCoord;
} PostAttribLocs;

typedef struct {
    GLint texture1;
} PostUniformLocs;

typedef struct {
    GLuint vbo;
    GLuint ibo;
    int indexCnt;
} PostGeometry;

typedef struct {
    GLuint fbo;
    GLuint color;
} PostFramebuffer;

typedef struct {
    float pos[3];
    float tex[2];
} PostVertex;

TestError SetupColorFbo(PostFramebuffer* fb);

TestError DeletePostFramebuffer(PostFramebuffer* fb);

TestError BufferPostVertices();

TestError CompileSimplePostShaders(ShaderPair* priv);
TestError CompilePostShaders(ShaderPair* priv);

TestError GetPostUniformAndAttribLocs(GLuint prg,
        PostAttribLocs* attribs,
        PostUniformLocs* uniforms);

TestError BindPostShader(
        GLuint prg,
        GLuint tex,
        PostAttribLocs* attribs,
        PostUniformLocs* uniforms);
