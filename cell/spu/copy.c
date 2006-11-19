#include "fftw-spu.h"
#include "../fftw-cell.h"

#if FFTW_SINGLE
typedef unsigned long long complex_hack_t;
#else
typedef vector float complex_hack_t;
#endif

void X(spu_complex_memcpy)(R *dst, int dstride,
			   const R *src, int sstride,
			   int n)
{
     int i;

     for (i = 0; i < n - 3; i += 4, src += 4 * sstride, dst += 4 * dstride) {
	  complex_hack_t x0 = *((complex_hack_t *)(src + 0 * sstride));
	  complex_hack_t x1 = *((complex_hack_t *)(src + 1 * sstride));
	  complex_hack_t x2 = *((complex_hack_t *)(src + 2 * sstride));
	  complex_hack_t x3 = *((complex_hack_t *)(src + 3 * sstride));
	  *((complex_hack_t *)(dst + 0 * dstride)) = x0;
	  *((complex_hack_t *)(dst + 1 * dstride)) = x1;
	  *((complex_hack_t *)(dst + 2 * dstride)) = x2;
	  *((complex_hack_t *)(dst + 3 * dstride)) = x3;
     }
     for (; i < n; ++i, src += sstride, dst += dstride) {
	  complex_hack_t x0 = *((complex_hack_t *)(src + 0 * sstride));
	  *((complex_hack_t *)(dst + 0 * dstride)) = x0;
     }
}

void X(spu_complex_transpose)(R *A_, int n)
{
     int i, j;
     complex_hack_t *A = (complex_hack_t *)A_;

     for (i = 0; i < n; ++i) 
	  for (j = i + 1; j < n; ++j) {
	       complex_hack_t aij = A[(i * n + j)];
	       complex_hack_t aji = A[(j * n + i)];
	       A[(i * n + j)] = aji;
	       A[(j * n + i)] = aij;
	  }
}

/* A is ni rows x nj columns, row-major.
   B is nj rows x ni columns, row-major. */
void X(spu_complex_transpose_and_swap)(R *A_, R *B_, int ni, int nj)
{
     int i, j;
     complex_hack_t *A = (complex_hack_t *)A_;
     complex_hack_t *B = (complex_hack_t *)B_;

     for (i = 0; i < ni; ++i) {
	  for (j = 0; j < nj - 3; j += 4) {
	       complex_hack_t aij0 = A[(i * nj + j + 0)];
	       complex_hack_t aij1 = A[(i * nj + j + 1)];
	       complex_hack_t aij2 = A[(i * nj + j + 2)];
	       complex_hack_t aij3 = A[(i * nj + j + 3)];
	       complex_hack_t bj0i = B[((j + 0) * ni + i)];
	       complex_hack_t bj1i = B[((j + 1) * ni + i)];
	       complex_hack_t bj2i = B[((j + 2) * ni + i)];
	       complex_hack_t bj3i = B[((j + 3) * ni + i)];
	       B[((j + 0) * ni + i)] = aij0;
	       B[((j + 1) * ni + i)] = aij1;
	       B[((j + 2) * ni + i)] = aij2;
	       B[((j + 3) * ni + i)] = aij3;
	       A[(i * nj + j + 0)] = bj0i;
	       A[(i * nj + j + 1)] = bj1i;
	       A[(i * nj + j + 2)] = bj2i;
	       A[(i * nj + j + 3)] = bj3i;
	  }
	  for (; j < nj; ++j) {
	       complex_hack_t aij = A[(i * nj + j)];
	       complex_hack_t bji = B[(j * ni + i)];
	       A[(i * nj + j)] = bji;
	       B[(j * ni + i)] = aij;
	  }
     }
}