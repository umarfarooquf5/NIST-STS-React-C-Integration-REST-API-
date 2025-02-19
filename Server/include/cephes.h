#ifndef CEPHES_H
#define CEPHES_H

#include <cmath>

// Constants
extern double MACHEP;
extern double MAXLOG;
extern double MAXNUM;
extern double PI;

// Function Declarations
double cephes_igamc(double a, double x);
double cephes_igam(double a, double x);
double cephes_lgam(double x);
double cephes_polevl(double x, double *coef, int N);
double cephes_p1evl(double x, double *coef, int N);
double cephes_erf(double x);
double cephes_erfc(double x);
double cephes_normal(double x);


#endif // CEPHES_H
