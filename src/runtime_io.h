/*
 * runtime_io.h
 *
 * Declarations for runtime I/O functions used by the SIMPLES code generator.
 * These symbols are called from generated assembly and must use C linkage.
 *
 * Keep names exact (no mangling):
 *   int __read_int(void);
 *   void __write_int(int value);
 *   void __write_nl(void);
 *   double __read_float(void);
 *   void __write_float(double value);
 *
 * This header is safe to include from C or C++ (extern "C").
 */

#ifndef COMPILADOR_COP_RUNTIME_IO_H
#define COMPILADOR_COP_RUNTIME_IO_H

#ifdef __cplusplus
extern "C" {
#endif

int __read_int(void);
void __write_int(int value);
void __write_nl(void);
double __read_float(void);
void __write_float(double value);

#ifdef __cplusplus
}
#endif

#endif /* COMPILADOR_COP_RUNTIME_IO_H */
