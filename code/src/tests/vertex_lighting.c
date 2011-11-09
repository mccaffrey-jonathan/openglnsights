#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"
#include "common/scene_setup.h"
#include <stdlib.h>

typedef struct {
    ShaderPair shads;
    SceneFramebuffer fb;
    Geometry geo;
    SceneAttribLocs sceneAttribLocs;
    SceneUniformLocs sceneUniformLocs;
    GLuint tex;
} Private;

static TestError setup(TestData* data)
{
    Private* priv = malloc(sizeof(Private));
    if (!priv)
        return OUT_OF_MEMORY;
    data->priv = priv;
    TestError err = SUCCESS;

    err = SetupDepthAndColorFbo(&priv->fb);
    if (err != SUCCESS)
        return err;

    err = CompileSceneShaders(&priv->shads);
    if (err != SUCCESS)
        return err;

    err = GetSceneUniformAndAttribLocs(priv->shads.prg,
            &priv->sceneAttribLocs,
            &priv->sceneUniformLocs);
    if (err != SUCCESS)
        return err;

    err = GenAndBindBuffers(&priv->geo);
    if (err != SUCCESS)
        return err;

    priv->geo.indexCnt = (SCENE_VERTEX_HEIGHT-1)*(SCENE_VERTEX_WIDTH-1)*6;
    err = bufferDataSceneVertexGrid(SCENE_VERTEX_WIDTH, SCENE_VERTEX_HEIGHT); 
    if (err != SUCCESS)
        return err;

    err = CreateTexture(&priv->tex);
    if (err != SUCCESS)
        return err;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(1.0f, 0.0f, 0.5f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    return SUCCESS;
}

static void DrawFrame(Private* priv)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    BindSceneShader(priv->shads.prg,
            priv->tex,
            &priv->sceneAttribLocs,
            &priv->sceneUniformLocs);
    DrawGeo(&priv->geo);
    glFlush();
}

static TestError warmup(const TestData* data)
{
    Private* priv = (Private*) data->priv;
    for (int i = 0; i < WARMUP_ITERS; i++) {
        DrawFrame(priv);
    }
    glFinish();
    return SUCCESS;
}


static TestError run(TestData* data)
{
    Private* priv = (Private*) data->priv;
    for (int i = 0; i < TEST_ITERS; i++) {
       DrawFrame(priv);
       glFinish();
    }
    return SUCCESS;
}

static TestError report(const TestData* data, TestReport* out)
{
    out->unit = PIXELS;
    out->count = WIDTH*HEIGHT*TEST_ITERS;
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
    DeleteGeometry(&priv->geo);
    DeleteShaderPair(&priv->shads);
    glDeleteTextures(1, &priv->tex);
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
