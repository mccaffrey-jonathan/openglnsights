#include "common/test.h"

static TestError setup(TestData* data)
{
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

const TestCase trivial_pass =
{
    .name = "trivial_pass",
    .setup = setup,
    .warmup = warmup,
    .run = run,
    .report = report,
    .teardown = teardown,
};
