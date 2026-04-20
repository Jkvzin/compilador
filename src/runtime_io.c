/*
 * runtime_io.c
 *
 * Portable C implementations of runtime I/O functions used by the code
 * generator. These are simple wrappers around standard I/O (scanf/printf)
 * so generated assembly can call them (cdecl / C linkage).
 *
 * Uses C99. Keep symbol names exactly as declared in runtime_io.h.
 */

#include <stdio.h>
#include "runtime_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Read an integer from stdin. On failure (e.g. EOF or parse error) returns 0.
 * Uses scanf with "%%d" which expects an int pointer.
 */
int __read_int(void) {
    int value = 0;
    if (scanf("%d", &value) == 1) {
        return value;
    }
    /* On failure return 0 as a safe default. */
    return 0;
}

/* Write an integer to stdout (no newline) and flush stdout. */
void __write_int(int value) {
    printf("%d", value);
    fflush(stdout);
}

/* Write a newline to stdout and flush. */
void __write_nl(void) {
    putchar('\n');
    fflush(stdout);
}

/*
 * Read a double (float) from stdin. On failure returns 0.0.
 * Uses scanf with "%%lf" which expects a double pointer.
 */
double __read_float(void) {
    double value = 0.0;
    if (scanf("%lf", &value) == 1) {
        return value;
    }
    return 0.0;
}

/* Write a double using %%g (compact format) and flush stdout. */
void __write_float(double value) {
    printf("%g", value);
    fflush(stdout);
}

#ifdef __cplusplus
}
#endif
