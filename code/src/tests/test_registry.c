#include "common/test.h"

extern const TestCase trivial_pass;
extern const TestCase vertex_lighting;
extern const TestCase fragment_lighting;
extern const TestCase five_tap_post;
extern const TestCase one_tap_post;
extern const TestCase triangle_throughput;
extern const TestCase cpu_writes;
extern const TestCase cpu_read_accum;
extern const TestCase clears;

const TestCase* suite [] = {
    &trivial_pass,
    &one_tap_post,
    &five_tap_post,
    &vertex_lighting,
    &fragment_lighting,
    &clears,
    &cpu_writes,
    &cpu_read_accum,
};

const uint32_t num_tests = sizeof(suite) / sizeof(const TestCase*);
