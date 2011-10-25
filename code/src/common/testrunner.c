#include "common/test.h"

static void PrintResult(const TestCase* test,
        const TestResult* res,
        FILE* output)
{
    fprintf(output, "%s: ", test->name);
    //TODO improve error reporting
    if (res->err == SUCCESS) {
        fprintf(output, "%s", "SUCCESS");
    } else {
        fprintf(output, "%s", "TODO, improve error reporting");
    }
    fprintf(output, "%s", "\n");
}

static TestError RunTest(const TestCase* test)
{
    TestData data;
    TestError err = Success;
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

    err = test->teardown(&data);
    if (err != SUCCESS)
        goto finish;

finish:

    TestResult res = {
        .report = work;
        .elapsed = elapsed;
        .err = err;
    };
    return res;
}

static const TestCase* suite [] = {

};

static const uint32_t num_tests = sizeof(suite) / sizeof(const TestCase*);

void RunAllTests(FILE* output)
{
    for (uint32_t i = 0; i < num_tests; i++) {
       TestResult res = RunTest(suite[i]);
       PrintResult(suite[i], &res, output);
    }
}
