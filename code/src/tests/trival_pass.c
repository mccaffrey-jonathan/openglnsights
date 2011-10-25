#include "common/test.h"

static TestError setup(TestData* data)
{
    return SUCCESS;
}

static TestError warmup(TestData* data)
{
    return SUCCESS;
}

static TestError run(TestData* data)
{
    return SUCCESS;
}

static TestError report(TestData* data)
{
    return SUCCESS;
}

static TestError teardown(TestData* data)
{
    return SUCCESS;
}

TestCase trivial_pass =
{
    .name = "trivial_pass",
    .setup = setup;
    .warmup = warmup;
    .run = run;
    .report = report;
    .teardown = teardown;
};
