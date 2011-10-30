#include "common/test.h"
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE (10 * 1024 * 1024) //To defeat L2 caches
#define WARMUP_ITERS 10
#define TEST_ITERS 1024LL

typedef struct {
    void* buff;
} Private;

static TestError setup(TestData* data)
{
    Private* priv = malloc(sizeof(Private));
    if (!priv)
        return OUT_OF_MEMORY;
    data->priv = priv;
    priv->buff = malloc(BUFF_SIZE);
    if (!priv->buff)
        return OUT_OF_MEMORY;
    return SUCCESS;
}

static TestError warmup(const TestData* data)
{
    if (!data)
        return NULL_POINTER;
    Private* priv = (Private*)data->priv;
    if (!priv)
        return NULL_POINTER;
    if (!priv->buff)
        return NULL_POINTER;
    for (int i = 0; i < WARMUP_ITERS; i++) {
        memset(priv->buff, 0xBE, BUFF_SIZE);
    }
    return SUCCESS;
}

static TestError run(TestData* data)
{
    if (!data)
        return NULL_POINTER;
    Private* priv = (Private*)data->priv;
    if (!priv)
        return NULL_POINTER;
    if (!priv->buff)
        return NULL_POINTER;
    for (int i = 0; i < TEST_ITERS; i++) {
        memset(priv->buff, 0xEF, BUFF_SIZE);
    }
    return SUCCESS;
}

static TestError report(const TestData* data, TestReport* out)
{
    out->unit = BYTES;
    out->count = TEST_ITERS*BUFF_SIZE;
    return SUCCESS;
}

static TestError teardown(TestData* data)
{
    if (!data)
        return NULL_POINTER;
    Private* priv = (Private*)data->priv;
    if (!priv)
        return NULL_POINTER;
    void* buff = priv->buff;
    priv->buff = NULL;
    free(priv);
    data->priv = NULL;
    if (!buff) {
        return NULL_POINTER;
    }
    free(buff);
    return SUCCESS;
}

const TestCase cpu_writes =
{
    .name = "cpu_writes",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
