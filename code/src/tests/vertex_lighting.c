#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"

#define WIDTH 1024
#define HEIGHT 1024
#define VERTEX_WIDTH 250
#define VERTEX_HEIGHT 250

typedef struct {
    GLuint fbo;
    GLuint depth;
    GLuint color;
    GLuint vbo;
    GLuint ibo;
    GLuint vs;
    GLuint fs;
    GLuint prg;
} Private;

static void SetupDepthAndColorFbo(Private* priv) 
{
    glGenFramebuffers(1, &priv->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo);
    glGenRenderbuffers(1, &priv->color);
    glBindRenderbuffer(GL_RENDERBUFFER, priv->color);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_RGB5_A1, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER,
            priv->color);
    glGenRenderbuffers(1, &priv->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, priv->depth);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            priv->depth);
}

typedef struct {
    float pos[3];
    float nrm[3];
    float tex[2];
} SceneVertex;

static TestError bufferDataSceneVertexGrid(int width, int height) {

    SceneVertex* verts = malloc(width*height*sizeof(SceneVertex));
    if (!verts)
        return OUT_OF_MEMORY;

    //Setup grid of vertices
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            verts[i*height + j] = (SceneVertex)
            {
                .pos = {((float)i)/width, ((float)j)/height, 1.0f},
                .nrm = {0.0f, 0.0f, 1.0f},
                .tex = {((float)i)/width, ((float)j)/height},
            };
        }
    }

    glBufferData(GL_ARRAY_BUFFER,
            width*height*sizeof(SceneVertex),
            verts,
            GL_STATIC_DRAW);

    free(verts);
    verts = NULL;

    int32_t* indices = malloc((width-1)*(height-1)*sizeof(int32_t)*6);

    //Setup indexed triangles
    for (int i = 0; i < (width-1); i++) {
        for (int j = 0; j < (height-1); j++) {
           indices[6*(i*height + j) + 0] = i*width + j;
           indices[6*(i*height + j) + 1] = i*height + j + 1;
           indices[6*(i*height + j) + 2] = (i+1)*height + j;

           indices[6*(i*height + j) + 3] = i*height + j + 1;
           indices[6*(i*height + j) + 5] = (i+1)*height + j + 1;
           indices[6*(i*height + j) + 4] = (i+1)*height + j;
        }
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (width-1)*(height-1)*sizeof(int32_t)*6,
            indices,
            GL_STATIC_DRAW);
    
    free(indices);
    indices = NULL;

    return SUCCESS;
}

// from http://code.google.com/p/androidshaders/source/browse/res/raw/gouraud_vs.txt
// Vertex shader Gouraud Shading - Per-vertex lighting
GLchar vtx [] =
"precision mediump float;\n"
"uniform mat4 uMVPMatrix;\n"
"uniform vec4 lightPos;\n"
"uniform vec4 matDiffuse;\n"
"\n"
"attribute vec4 aPosition;\n"
"attribute vec3 aNormal; \n"
"attribute vec2 textureCoord;\n"
"\n"
"varying vec2 tCoord;\n"
"varying vec4 color;\n"
"\n"
"void main() {\n"
"    // pass on texture variables\n"
"    tCoord = textureCoord;\n"
"    // normal\n"
"\n"
"    // the vertex position\n"
"    vec4 posit = uMVPMatrix * aPosition; \n"
"\n"
"    // light dir\n"
"    vec3 lightDir = lightPos.xyz - posit.xyz;\n"
"    vec3 EyespaceNormal = vec3(uMVPMatrix * vec4(aNormal, 1.0));\n"
"    vec3 N = normalize(EyespaceNormal);\n"
"    vec3 L = normalize(lightDir);\n"
"    \n"
"    vec4 diffuseTerm = matDiffuse * max(dot(N, L), 0.0);\n"
"    \n"
"    color = diffuseTerm;\n"
"	\n"
"    gl_Position = posit; \n"
"}\n"
;

GLchar frg [] =
"precision mediump float;\n"
"\n"
"uniform sampler2D texture1; // color texture\n"
"\n"
"varying vec2 tCoord;\n"
"varying vec4 color;\n"
"\n"
"void main() {\n"
"    gl_FragColor = color*texture2D(texture1, tCoord);\n"
"}\n"
;


static TestError CompileSceneShaders(Private* priv)
{
    priv->vs = glCreateShader(GL_VERTEX_SHADER);
    int len = sizeof(vtx);
    const GLchar* vtxSrc = vtx;
    glShaderSource(priv->vs, 1, &vtxSrc, &len);
    glCompileShader(priv->vs);
    TestError err = checkAndReportCompilationStatus(priv->vs);
    if (err != SUCCESS)
        return err;

    priv->fs = glCreateShader(GL_FRAGMENT_SHADER);
    len = sizeof(frg);
    const GLchar* frgSrc = frg;
    glShaderSource(priv->fs, 1, &frgSrc, &len);
    glCompileShader(priv->fs);
    err = checkAndReportCompilationStatus(priv->fs);
    if (err != SUCCESS)
        return err;

    priv->prg = glCreateProgram();
    glAttachShader(priv->prg, priv->vs);
    glAttachShader(priv->prg, priv->fs);
    glLinkProgram(priv->prg);
    checkAndReportLinkStatus(priv->prg);

    return SUCCESS;
}

static TestError setup(TestData* data)
{
    Private* priv = malloc(sizeof(Private));
    if (!priv)
        return OUT_OF_MEMORY;
    data->priv = priv;

    SetupDepthAndColorFbo(priv);
    TestError err = checkAndReportFramebufferStatus();

    if (err != SUCCESS)
        return err;

    glGenBuffers(1, &priv->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, 
            priv->vbo);

    glGenBuffers(1, &priv->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
            priv->ibo);

    err = bufferDataSceneVertexGrid(VERTEX_WIDTH, VERTEX_HEIGHT); 

    if (err != SUCCESS)
        return err;

    err = CompileSceneShaders(priv);
    if (err != SUCCESS)
        return err;

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WIDTH, HEIGHT);

    return SUCCESS;
}

static TestError warmup(const TestData* data)
{
    return SUCCESS;
}

static TestError run(TestData* data)
{
    return SUCCESS;
}

static TestError report(const TestData* data, TestReport* out)
{
    out->unit = INVALID;
    out->count = 0;
    return SUCCESS;
}

static TestError teardown(TestData* data)
{
    return SUCCESS;
}

const TestCase vertex_lighting =
{
    .name = "vertex_lighting",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
