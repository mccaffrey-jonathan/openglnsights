#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"

//#define WIDTH 1024
//#define HEIGHT 1024
#define WIDTH 10
#define HEIGHT 10 
#define BYTES_PER_PIXEL 2
#define VERTEX_WIDTH 250
#define VERTEX_HEIGHT 250

#define WARMUP_ITERS 10
#define TEST_ITERS (10 * 1024LL) / BYTES_PER_PIXEL

typedef struct {
    GLint aPosition;
    GLint aNormal;
    GLint textureCoord;
} SceneAttribLocs;

typedef struct {
    GLint uMVPMatrix;
    GLint lightPos;
    GLint matDiffuse;
} SceneUniformLocs;

typedef struct {
    GLuint vbo;
    GLuint ibo;
    int indexCnt;
} SceneGeometry;

typedef struct {
    GLuint fbo;
    GLuint depth;
    GLuint color;
} SceneFramebuffer;

typedef struct {
    GLuint vs;
    GLuint fs;
    GLuint prg;
} ShaderPair;

typedef struct {
    float pos[3];
    float nrm[3];
    float tex[2];
} SceneVertex;

typedef struct {
    ShaderPair shads;
    SceneFramebuffer fb;
    SceneGeometry geo;
    SceneAttribLocs sceneAttribLocs;
    SceneUniformLocs sceneUniformLocs;
} Private;

// from http://code.google.com/p/androidshaders/source/browse/res/raw/gouraud_vs.txt
// Vertex shader Gouraud Shading - Per-vertex lighting
const GLfloat IDENTITY_MAT4 [] = 
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

//These don't really matter
const GLfloat LIGHT_POS [] =
    { 1.0f, 1.0f, 1.0f, 1.0f };

const GLfloat MAT_DIFFUSE [] =
    { 1.0f, 1.0f, 1.0f, 1.0f };


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
"//    gl_FragColor = color*texture2D(texture1, tCoord);\n"
"    gl_FragColor = 0.000*color+vec4(0.0, 0.3, 1.0, 1.0);\n"
"}\n"
;

static TestError SetupDepthAndColorFbo(SceneFramebuffer* fb)
{
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    glGenRenderbuffers(1, &fb->color);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->color);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_RGB5_A1, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER,
            fb->color);
    glGenRenderbuffers(1, &fb->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->depth);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            fb->depth);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    return SUCCESS;
}

static TestError bufferDataSceneVertexGrid(int width, int height) {

    SceneVertex* verts = malloc(width*height*sizeof(SceneVertex));
    if (!verts)
        return OUT_OF_MEMORY;

    //Setup grid of vertices
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            verts[i*height + j] = (SceneVertex)
            {
                .pos = {((float)i)/width, ((float)j)/height, 0.5f},
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

    uint32_t* indices = malloc((width-1)*(height-1)*sizeof(uint32_t)*6);

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
            (width-1)*(height-1)*sizeof(uint32_t)*6,
            indices,
            GL_STATIC_DRAW);
    
    free(indices);
    indices = NULL;

    return SUCCESS;
}



static TestError CompileSceneShaders(ShaderPair* priv)
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
    err = checkAndReportLinkStatus(priv->prg);
    if (err != SUCCESS)
        return err;

    return SUCCESS;
}

#define ATTRIB(PRG, X) .X = glGetAttribLocation(PRG, #X)
#define UNIFOR(PRG, X) .X = glGetUniformLocation(PRG, #X)

static TestError GetSceneUniformAndAttribLocs(GLuint prg,
        SceneAttribLocs* attribs,
        SceneUniformLocs* uniforms)
{
    *attribs = (SceneAttribLocs){
        ATTRIB(prg, aPosition),
        ATTRIB(prg, aNormal),
        ATTRIB(prg, textureCoord),
    };

    *uniforms = (SceneUniformLocs){
        UNIFOR(prg, uMVPMatrix),
        UNIFOR(prg, lightPos),
        UNIFOR(prg, matDiffuse),
    };
    fprintf(stderr, "%d %d %d",
            attribs->aPosition,
            attribs->aNormal,
            attribs->textureCoord);

    if (attribs->aPosition == -1 ||
            attribs->aNormal == -1 ||
            attribs->textureCoord == -1) {
        return INVALID_ATTRIBUTE;
    }

    if (uniforms->uMVPMatrix == -1 ||
            uniforms->lightPos == -1 ||
            uniforms->matDiffuse == -1) {
        return INVALID_UNIFORM;
    }

    return SUCCESS;
}

#undef ATTRIB
#undef UNIFOR

#define SIZEOF_FIELD(TYPE, FIELD) sizeof(((TYPE*)0)->FIELD)

static TestError BindSceneShader(
        GLuint prg,
        SceneAttribLocs* attribs,
        SceneUniformLocs* uniforms)
{
    glUseProgram(prg);
    glVertexAttribPointer(attribs->aPosition,
            SIZEOF_FIELD(SceneVertex, pos)/sizeof(GL_FLOAT),
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneVertex),
            0);
    glVertexAttribPointer(attribs->aNormal,
            SIZEOF_FIELD(SceneVertex, nrm),
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneVertex),
            (void*)SIZEOF_FIELD(SceneVertex, pos));
    glVertexAttribPointer(attribs->textureCoord,
            SIZEOF_FIELD(SceneVertex, tex),
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneVertex),
            (void*)(SIZEOF_FIELD(SceneVertex, pos) +
                SIZEOF_FIELD(SceneVertex, nrm)));
    
    glUniformMatrix4fv(uniforms->uMVPMatrix,
            1,
            GL_FALSE,
            IDENTITY_MAT4);
    glUniform4fv(uniforms->lightPos, 1, LIGHT_POS);
    glUniform4fv(uniforms->matDiffuse, 1, MAT_DIFFUSE);
    return SUCCESS;
}

#undef SIZEOF_FIELD

static TestError GenAndBindBuffers(SceneGeometry* geo) {
    glGenBuffers(1, &geo->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, 
            geo->vbo);

    glGenBuffers(1, &geo->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 
            geo->ibo);
    return SUCCESS;
}

static TestError setup(TestData* data)
{
    fprintf(stderr, "in setyup\n");
    Private* priv = malloc(sizeof(Private));
    if (!priv)
        return OUT_OF_MEMORY;
    data->priv = priv;
    TestError err = SUCCESS;
    fprintf(stderr, "Alloced privat\n");

    err = SetupDepthAndColorFbo(&priv->fb);
    if (err != SUCCESS)
        return err;
    fprintf(stderr, "fbo setup\n");

    err = checkAndReportFramebufferStatus();
    if (err != SUCCESS)
        return err;

    err = CompileSceneShaders(&priv->shads);
    if (err != SUCCESS)
        return err;
    fprintf(stderr, "shaders compiled\n");

    err = GetSceneUniformAndAttribLocs(priv->shads.prg,
            &priv->sceneAttribLocs,
            &priv->sceneUniformLocs);
    if (err != SUCCESS)
        return err;

    fprintf(stderr, "get uniform and attrib locs\n");

    err = GenAndBindBuffers(&priv->geo);
    if (err != SUCCESS)
        return err;

    fprintf(stderr, "genandbindbuffers\n");

    priv->geo.indexCnt = (VERTEX_HEIGHT-1)*(VERTEX_WIDTH-1)*6;
    err = bufferDataSceneVertexGrid(VERTEX_WIDTH, VERTEX_HEIGHT); 
    if (err != SUCCESS)
        return err;

    fprintf(stderr, "bufferdatascenevertexgrid\n");

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 0.0f, 0.5f, 1.0f);
    glClearDepth(1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);
    
    fprintf(stderr, "%d", glGetError());

    return SUCCESS;
}

static void DrawScene(SceneGeometry* buffs)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffs->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffs->ibo);
    glDrawElements(GL_TRIANGLES,
            buffs->indexCnt,
            GL_UNSIGNED_INT,
            0);
}

static void DrawFrame(Private* priv)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    BindSceneShader(priv->shads.prg,
            &priv->sceneAttribLocs,
            &priv->sceneUniformLocs);
    DrawScene(&priv->geo);
    //For parity with other tests
    glUseProgram(0);
}

static TestError warmup(const TestData* data)
{
    Private* priv = (Private*) data->priv;
    for (int i = 0; i < WARMUP_ITERS; i++) {
        DrawFrame(priv);
    }
    return SUCCESS;
}

static void DebugDumpPixels()
{
    GLfloat* pixs = malloc(sizeof(GLfloat)*4*WIDTH*HEIGHT);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0,
            WIDTH,
            HEIGHT,
            GL_RGB,
            GL_FLOAT,
            pixs);
    for (int i = 0; i < WIDTH*HEIGHT*3; i+=3) {
        fprintf(stderr, "%d %f %f %f\n", i/3, 
                pixs[i+0], pixs[i+1],pixs[i+2]);
    }
    free(pixs);
}

static TestError run(TestData* data)
{
    Private* priv = (Private*) data->priv;
    for (int i = 0; i < TEST_ITERS; i++) {
       DrawFrame(priv);
    }
    DebugDumpPixels();
    return SUCCESS;
}

static TestError report(const TestData* data, TestReport* out)
{
    out->unit = PIXELS;
    out->count = WIDTH*HEIGHT*TEST_ITERS;
    return SUCCESS;
}

static TestError DeleteSceneFramebuffer(SceneFramebuffer* fb) 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &fb->color);
    glDeleteRenderbuffers(1, &fb->depth);
    glDeleteFramebuffers(1, &fb->fbo);
    return SUCCESS;
}

static TestError DeleteSceneGeometry(SceneGeometry* geo) 
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &geo->vbo);
    glDeleteBuffers(1, &geo->ibo);
    return SUCCESS;
}

static TestError DeleteShaderPair(ShaderPair* shads) {
    glDeleteShader(shads->vs);
    glDeleteShader(shads->fs);
    glDeleteProgram(shads->prg);
    return SUCCESS;
}

static TestError teardown(TestData* data)
{
    if (!data)
        return NULL_POINTER;
    Private* priv = data->priv;
    if (!priv)
        return NULL_POINTER;
    DeleteSceneFramebuffer(&priv->fb);
    DeleteSceneGeometry(&priv->geo);
    DeleteShaderPair(&priv->shads);
    free(priv);
    data->priv = NULL;
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
