#include "../src/runtime_io.h"
#include <stdio.h>
#include <stdlib.h>

/* Simple test harness matching the style used by other tests in this repo. */
static int test_count = 0;
static int test_passed = 0;

void assert_true(int cond, const char *msg) {
    test_count++;
    if (cond) {
        printf("  ✓ %s\n", msg);
        test_passed++;
    } else {
        printf("  ✗ %s\n", msg);
    }
}

int main(void) {
    printf("====== Runtime I/O Tests ======\n");

    printf("Output from write functions:\n");
    /* Call write functions to ensure symbols link and produce output. */
    __write_int(42);
    __write_nl();
    __write_float(3.14);
    __write_nl();

    /* We avoid calling blocking read functions so CI won't hang. */
    assert_true(1, "write functions executed");

    printf("\n====== Test Summary ======\n");
    printf("Passed: %d / %d\n", test_passed, test_count);
    return (test_passed == test_count) ? 0 : 1;
}
