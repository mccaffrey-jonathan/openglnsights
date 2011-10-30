#include "common/test.h"

extern const TestCase trivial_pass;
extern const TestCase triangle_throughput;
extern const TestCase cpu_writes;
extern const TestCase clears;

const TestCase* suite [] = {
    &trivial_pass,
    &triangle_throughput,
    &cpu_writes,
    &clears,
};

const uint32_t num_tests = sizeof(suite) / sizeof(const TestCase*);
