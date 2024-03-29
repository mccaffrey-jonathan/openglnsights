#include "common/test.h"
#include "platform/time.h"
#include "platform/gl.h"

const char* errstrs [] = {
    "SUCCESS",
    "FEATURE_UNSUPPORTED",
    "OUT_OF_MEMORY",
    "NULL_POINTER",
    "OUT_OF_BOUNDS",
    "INIT_FAILED",
    "SHADER_COMPILATION_FAILED",
    "SHADER_LINK_FAILED",
    "INVALID_ATTRIBUTE",
    "INVALID_UNIFORM",
    "UNKNOWN_ERROR",
};
    

static void PrintResult(const TestCase* test,
        const TestResult* res,
        FILE* output)
{
    LOGI("Printing results");
    fprintf(output, "%s: ", test->name);
    //TODO improve error reporting
    if (res->err == SUCCESS) {
        //TODO add units
        LOGI("%s %s %lld in %lld ms",
                test->name,
                "SUCCESS",
                res->report.count,
                res->elapsed);
        fprintf(output, "%s %lld in %lld ms",
                "SUCCESS",
                res->report.count,
                res->elapsed);
    } else {
        LOGI("ERROR %s", errstrs[res->err]);
        fprintf(output, "ERROR %s", errstrs[res->err]);
    }
    fprintf(output, "%s", "\n");
}

static TestResult RunTest(const TestCase* test)
{
    LOGI("Starting %s", test->name);
    TestData data = {
        .priv = NULL,
    };
    TestError err = SUCCESS;
    int64_t elapsed = 0;
    TestReport work = {
       .unit = INVALID, 
       .count = 0,
    };

    LOGI("Performing setup");
    err = test->setup(&data);
    if (err != SUCCESS)
        goto finish;
    LOGI("Performing warmup");
    err = test->warmup(&data);
    if (err != SUCCESS)
        goto finish;

    LOGI("Running");
    int64_t start = TimeMillis();
    err = test->run(&data);
    if (err != SUCCESS)
        goto finish;
    int64_t end = TimeMillis();
    elapsed = end - start;
    
    LOGI("Report");
    err = test->report(&data, &work);
    if (err != SUCCESS)
        goto finish;

finish:
    //Do not record err of teardown
    //Since we can successfully tear-down a broken set-up
    //Teardown should be very conservative
    LOGI("Teardown");
    test->teardown(&data);


    LOGI("complete");
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
    LOGI("Running all tests");

    TestError platform_err = GlPlatformSetup();
    if (platform_err != SUCCESS) {
        printf("%s", "Could not initialize OpenGL or windowing systems\n");
        return;
    }

    for (uint32_t i = 0; i < num_tests; i++) {
       TestResult res = RunTest(suite[i]);
       PrintResult(suite[i], &res, output);
    }
    fflush(output);
}
