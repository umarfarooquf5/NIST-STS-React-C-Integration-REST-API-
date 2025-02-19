#ifndef DFFT_H
#define DFFT_H

#ifdef __cplusplus
extern "C"
{
#endif

    // Real FFT functions
    void __ogg_fdrffti(int n, double *wsave, int *ifac);
    void __ogg_fdrfftf(int n, double *r, double *wsave, int *ifac);
    void __ogg_fdrfftb(int n, double *r, double *wsave, int *ifac);

    // Cosine quarter-wave FFT functions
    void __ogg_fdcosqi(int n, double *wsave, int *ifac);
    void __ogg_fdcosqf(int n, double *x, double *wsave, int *ifac);
    void __ogg_fdcosqb(int n, double *x, double *wsave, int *ifac);

#ifdef __cplusplus
}
#endif

#endif // DFFT_H
