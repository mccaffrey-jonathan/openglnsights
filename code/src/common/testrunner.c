#include "common/test.h"
#include "platform/time.h"
#include "platform/gl.h"

static void PrintResult(const TestCase* test,
        const TestResult* res,
        FILE* output)
{
    fprintf(output, "%s: ", test->name);
    //TODO improve error reporting
    if (res->err == SUCCESS) {
        //TODO add units
        fprintf(output, "%s %lld in %lld ms",
                "SUCCESS",
                res->report.count,
                res->elapsed);
    } else {
        fprintf(output, "%s", "TODO, improve error reporting");
    }
    fprintf(output, "%s", "\n");
}

static TestResult RunTest(const TestCase* test)
{
    TestData data = {
        .priv = NULL,
    };
    TestError err = SUCCESS;
    int64_t elapsed = 0;
    TestReport work = {
       .unit = INVALID, 
       .count = 0,
    };

    err = test->setup(&data);
    if (err != SUCCESS)
        goto finish;
    err = test->warmup(&data);
    if (err != SUCCESS)
        goto finish;

    int64_t start = TimeMillis();
    err = test->run(&data);
    if (err != SUCCESS)
        goto finish;
    int64_t end = TimeMillis();
    elapsed = end - start;
    
    err = test->report(&data, &work);
    if (err != SUCCESS)
        goto finish;

finish:
    //Do not record err of teardown
    //Since we can successfully tear-down a broken set-up
    //Teardown should be very conservative
    test->teardown(&data);


    return (TestResult)
    {
        .report = work,
        .elapsed = elapsed,
        .err = err,
    };
}

extern const TestCase* suite [];
extern const uint32_t num_tests;

void RunAllTests(FILE* output)
{

    TestError platform_err = GlPlatformSetup();
    if (platform_err != SUCCESS) {
        printf("%s", "Could not initialize OpenGL or windowing systems\n");
        return;
    }

    for (uint32_t i = 0; i < num_tests; i++) {
       TestResult res = RunTest(suite[i]);
       PrintResult(suite[i], &res, output);
    }
}
