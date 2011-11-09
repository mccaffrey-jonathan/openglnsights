#include "common/test.h"
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE (10 * 1024 * 1024) //To defeat L2 caches
#define WARMUP_ITERS 10
#define TEST_ITERS (1024LL/10)

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
    memset(priv->buff, 0x00, BUFF_SIZE);
    return SUCCESS;
}

//Out parameter should defeat optimizations
void readAccum(const int32_t* restrict buff,
        int elemCnt,
        int32_t* restrict out)
{
    int32_t accum0=0;
    int32_t accum1=0;
    int32_t accum2=0;
    int32_t accum3=0;
    int32_t accum4=0;
    int32_t accum5=0;
    int32_t accum6=0;
    int32_t accum7=0;
    //Assume a pretty high alignment here
    for (int i = 0; i < elemCnt; i+=8) {
        __builtin_prefetch(buff + i + 64);
        accum0 += buff[i+0];
        accum1 += buff[i+1];
        accum2 += buff[i+2];
        accum3 += buff[i+3];
        accum4 += buff[i+4];
        accum5 += buff[i+5];
        accum6 += buff[i+6];
        accum7 += buff[i+7];
    }
    *out = accum0 +
        accum1 +
        accum2 + 
        accum3 +
        accum4 + 
        accum5 +
        accum6 +
        accum7 ;
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
        int32_t out;
        readAccum(priv->buff, BUFF_SIZE/sizeof(int32_t), &out);
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
        int32_t out;
        readAccum(priv->buff, BUFF_SIZE/sizeof(int32_t), &out);
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

const TestCase cpu_read_accum =
{
    .name = "cpu_read_accum",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
