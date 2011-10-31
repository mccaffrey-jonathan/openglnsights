#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"

typedef struct {
    GLuint fbo;
    GLuint color;
} Private;

static TestError setup(TestData* data)
{
    Private* priv = malloc(sizeof(Private));
    if (!priv)
        return OUT_OF_MEMORY;
    data->priv = priv;

    glGenFramebuffers(1, &priv->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, priv->fbo);
    glGenRenderbuffers(1, &priv->color);
    glBindRenderbuffer(GL_RENDERBUFFER, priv->color);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_RGB5_A1, 1, 1);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER,
            priv->color);

    TestError err = checkAndReportFramebufferStatus();
    if (err != SUCCESS)
        return err;

    glDisable(GL_DEPTH_TEST);
    //Single pixel viewport
    glViewport(0, 0, 1, 1);

    fprintf(stderr, "Setup done!");
    return SUCCESS;
}

static TestError warmup(const TestData* data)
{
    //Let work finish so we don't pollute timing
    glFinish();
    return SUCCESS;
}

static TestError run(TestData* data)
{
    glFinish();
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
    if (!data)
        return NULL_POINTER;
    Private* priv = data->priv;
    if (!priv)
        return NULL_POINTER;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &priv->color);
    glDeleteFramebuffers(1, &priv->fbo);
    free(priv);
    data->priv = NULL;
}

const TestCase triangle_throughput =
{
    .name = "triangle_throughput",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
