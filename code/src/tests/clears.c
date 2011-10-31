#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"

#define WIDTH 1024
#define HEIGHT 1024
#define BYTES_PER_PIXEL 2

#define WARMUP_ITERS 10
#define TEST_ITERS ((10 * 1024LL) / BYTES_PER_PIXEL)

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
            GL_RGB5_A1, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER,
            priv->color);

    TestError err = checkAndReportFramebufferStatus();
    if (err != SUCCESS)
        return err;

    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, WIDTH, HEIGHT);

    return SUCCESS;
}

static void ClearAndWait(float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //Let's hope driver writers are honest here
    //If not, might have to start doing single-pixel readpixels
    glFinish();
}

static TestError warmup(const TestData* data)
{
    for (int i = 0; i < WARMUP_ITERS; i++) {
        float val = ((float)i)/WARMUP_ITERS;
        ClearAndWait(val, val, val);
    }
    //Let work finish so we don't pollute timing
    glFinish();
    return SUCCESS;
}

static TestError run(TestData* data)
{
    for (int i = 0; i < TEST_ITERS; i++) {
        float val = ((float)i)/WARMUP_ITERS;
        ClearAndWait(val, val, val);
    }
    glFinish();
    return SUCCESS;
}

static TestError report(const TestData* data, TestReport* out)
{
    out->unit = BYTES;
    out->count = TEST_ITERS*WIDTH*HEIGHT*BYTES_PER_PIXEL;
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

const TestCase clears =
{
    .name = "clears",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
