#ifndef COMMON_TEST_H
#define COMMON_TEST_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
    INVALID,
    TRIANGLES,
    PIXELS,
    BYTES,
} WorkUnit;

typedef enum {
    SUCCESS,
    FEATURE_UNSUPPORTED,
    OUT_OF_MEMORY,
    NULL_POINTER,
    OUT_OF_BOUNDS,
    INIT_FAILED,
    SHADER_COMPILATION_FAILED,
    SHADER_LINK_FAILED,
    UNKNOWN_ERROR,
} TestError;

typedef struct {
    void* priv;
} TestData;

typedef struct {
    WorkUnit unit;
    int64_t count;
} TestReport;

typedef struct {
    TestReport report;
    int64_t elapsed;
    TestError err;
} TestResult;

typedef TestError (*SetupFunc)(TestData* data);
typedef TestError (*WarmupFunc)(const TestData* data);
typedef TestError (*RunFunc)(TestData* data);
typedef TestError (*ReportFunc)(const TestData* data, TestReport* out);
typedef TestError (*TearDownFunc)(TestData* data);

typedef struct {
    const char* name;
    SetupFunc setup;
    WarmupFunc warmup;
    RunFunc run;
    ReportFunc report;
    TearDownFunc teardown;
} TestCase;

void RunAllTests(FILE* output);
    
#endif
