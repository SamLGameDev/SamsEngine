#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void exactinit(void);

double orient2d(double* pa, double* pb, double* pc);
double orient3d(double* pa, double* pb, double* pc, double* pd);
double incircle(double* pa, double* pb, double* pc, double* pd);
double insphere(double* pa, double* pb, double* pc, double* pd, double* pe);

#ifdef __cplusplus
}
#endif