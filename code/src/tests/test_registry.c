#include "common/test.h"

extern const TestCase trivial_pass;
extern const TestCase vertex_lighting;
extern const TestCase triangle_throughput;
extern const TestCase cpu_writes;
extern const TestCase cpu_read_accum;
extern const TestCase clears;

const TestCase* suite [] = {
    &trivial_pass,
    &vertex_lighting,
    &triangle_throughput,
    &cpu_writes,
    &cpu_read_accum,
    &clears,
};

const uint32_t num_tests = sizeof(suite) / sizeof(const TestCase*);
