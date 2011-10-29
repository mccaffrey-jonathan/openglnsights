#include "common/test.h"

extern const TestCase trivial_pass;
extern const TestCase triangle_throughput;

const TestCase* suite [] = {
    &trivial_pass,
    &triangle_throughput,
};

const uint32_t num_tests = sizeof(suite) / sizeof(const TestCase*);
