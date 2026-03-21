// DO NOT MARK 
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git
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
