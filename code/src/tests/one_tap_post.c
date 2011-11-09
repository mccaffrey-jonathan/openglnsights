#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"

#include "common/scene_setup.h"
#include "common/post_setup.h"
#include <stdlib.h>

typedef struct {
    ShaderPair shads;
    SceneFramebuffer fb;
    Geometry geo;
    SceneAttribLocs sceneAttribLocs;
    SceneUniformLocs sceneUniformLocs;
    GLuint tex;
} ScenePass;

typedef struct {
    ShaderPair shads;
    PostFramebuffer fb;
    Geometry geo;
    PostAttribLocs postAttribLocs;
    PostUniformLocs postUniformLocs;
    GLuint tex;
} PostPass;

typedef struct {
    ScenePass scene;
    PostPass post;
} Private;

static TestError SetupScene(ScenePass* priv)
{
    TestError err = SUCCESS;

    err = SetupDepthRBOAndColorTex(&priv->fb);
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

    return SUCCESS;
}

static TestError SetupPost(PostPass* priv)
{
    TestError err = SUCCESS;

    err = SetupColorFbo(&priv->fb);
    if (err != SUCCESS)
        return err;

    err = CompileSimplePostShaders(&priv->shads);
    if (err != SUCCESS)
        return err;

    err = GetPostUniformAndAttribLocs(priv->shads.prg,
            &priv->postAttribLocs,
            &priv->postUniformLocs);
    if (err != SUCCESS)
        return err;

    err = GenAndBindBuffers(&priv->geo);
    if (err != SUCCESS)
        return err;

    priv->geo.indexCnt = 6;

    err = BufferPostVertices(); 
    if (err != SUCCESS)
        return err;

    return SUCCESS;
}

static TestError setup(TestData* data)
{
    Private* priv = malloc(sizeof(Private));
    if (!priv)
        return OUT_OF_MEMORY;
    data->priv = priv;
    TestError err = SUCCESS;

    err = SetupScene(&priv->scene);
    if (err != SUCCESS)
        return err;

    err = SetupPost(&priv->post);
    if (err != SUCCESS)
        return err;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(1.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, WIDTH, HEIGHT);

    return SUCCESS;
}

static void DrawFrame(Private* priv)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 
           priv->scene.fb.fbo);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    BindBuffers(&priv->scene.geo);
    BindSceneShader(priv->scene.shads.prg,
            priv->scene.tex,
            &priv->scene.sceneAttribLocs,
            &priv->scene.sceneUniformLocs);
    DrawGeo(&priv->scene.geo);
    glBindFramebuffer(GL_FRAMEBUFFER,
            priv->post.fb.fbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    BindBuffers(&priv->post.geo);
    BindPostShader(priv->post.shads.prg,
            priv->scene.fb.color,
            &priv->post.postAttribLocs,
            &priv->post.postUniformLocs);
    DrawGeo(&priv->post.geo);
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
    //DebugDumpPixels();
    //DebugDumpTexture(priv->scene.fb.color, WIDTH, HEIGHT);
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
    DeleteSceneFramebufferWithTex(&priv->scene.fb);
    DeleteGeometry(&priv->scene.geo);
    DeleteShaderPair(&priv->scene.shads);
    glDeleteTextures(1, &priv->scene.tex);
    DeleteGeometry(&priv->post.geo);
    DeleteShaderPair(&priv->post.shads);
    DeletePostFramebuffer(&priv->post.fb);
    free(priv);
    data->priv = NULL;
    return SUCCESS;
}

const TestCase one_tap_post =
{
    .name = "one_tap_post",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
