/*
 * Copyright (c) 2008-2010
 *	Nakata, Maho
 * 	All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include <mblas.h>
#include <blas.h>
#include <mpack_debug.h>

#if defined VERBOSE_TEST
#include <iostream>
#endif

#define MIN_N -2
#define MAX_N 8
#define MIN_M -2
#define MAX_M 8
#define MIN_LDA -2
#define MAX_LDA 8
#define MIN_LDB -2
#define MAX_LDB 8
#define MAX_ITER 3

REAL_REF maxdiff = 0.0;

void Rtrsm_test3(const char *side, const char *uplo, const char *transa, const char *diag, REAL_REF alpha_ref, REAL alpha)
{
    int errorflag = FALSE;
    int mpack_errno1, mpack_errno2;
    for (int m = MIN_M; m < MAX_M; m++) {
	for (int n = MIN_N; n < MAX_N; n++) {
	    int minlda;
	    if (Mlsame(side, "L"))
		minlda = max(1, m);
	    else
		minlda = max(1, n);
	    for (int lda = minlda; lda < MAX_LDA; lda++) {
		for (int ldb = max(1, m); ldb < MAX_LDB; ldb++) {
		    int k = 0;
		    if (Mlsame(side, "L"))
			k = m;
		    if (Mlsame(side, "R"))
			k = n;
#if defined VERBOSE_TEST
		    printf("#n is %d, m is %d, lda is %d, ldb is %d\n", n, m, lda, ldb);
		    printf("#side is %s, uplo is %s transa is %s\n", side, uplo, transa);
#endif
		    REAL_REF *A_ref;
		    REAL_REF *B_ref;
		    REAL *A;
		    REAL *B;

		    A_ref = new REAL_REF[matlen(lda, k)];
		    B_ref = new REAL_REF[matlen(ldb, n)];
		    A = new REAL[matlen(lda, k)];
		    B = new REAL[matlen(ldb, n)];

		    for (int iter = 0; iter < MAX_ITER; iter++) {
			set_random_vector(A_ref, A, matlen(lda, k));
			set_random_vector(B_ref, B, matlen(ldb, n));

			mpack_errno = 0; blas_errno = 0;
#if defined ___MPACK_BUILD_WITH_MPFR___
			dtrsm_f77(side, uplo, transa, diag, &m, &n, &alpha_ref, A_ref, &lda, B_ref, &ldb);
			mpack_errno1 = blas_errno;
#else
			Rtrsm(side, uplo, transa, diag, m, n, alpha_ref, A_ref, lda, B_ref, ldb);
			mpack_errno1 = mpack_errno;
#endif
			Rtrsm(side, uplo, transa, diag, m, n, alpha, A, lda, B, ldb);
			mpack_errno2 = mpack_errno;

#if defined VERBOSE_TEST
			printf("errno: mpack %d, ref %d\n", mpack_errno1, mpack_errno2);
#endif
			if (mpack_errno1 != mpack_errno2) {
			    printf("error in Mxerbla!!\n");
			    exit(1);
			}
			REAL_REF diff = infnorm(B_ref, B, matlen(ldb, n), 1);
			if (diff > EPSILON12) {
			    printf("error: "); printnum(diff); printf("\n");
			    errorflag = TRUE;
			}
			if (maxdiff < diff)
			    maxdiff = diff;
			printf("max error: "); printnum(maxdiff); printf("\n");

		    }
		    delete[]B_ref;
		    delete[]A_ref;
		    delete[]B;
		    delete[]A;
		}
	    }
	}
    }
    if (errorflag == TRUE) {
	printf("Rtrsm test failed...\n");
	exit(1);
    }
    printf("max error: "); printnum(maxdiff); printf("\n");
}

void Rtrsm_test2(const char *side, const char *uplo, const char *transa, const char *diag)
{
    REAL_REF alpha_ref;
    REAL alpha;

//a=*
    set_random_number(alpha_ref, alpha);
    Rtrsm_test3(side, uplo, transa, diag, alpha_ref, alpha);

//a=1
    alpha_ref = 1.0;
    alpha = 1.0;
    Rtrsm_test3(side, uplo, transa, diag, alpha_ref, alpha);

//a=0
    alpha_ref = 0.0;
    alpha = 0.0;
    Rtrsm_test3(side, uplo, transa, diag, alpha_ref, alpha);
}

void Rtrsm_test()
{
    Rtrsm_test2("L", "U", "N", "U");
    Rtrsm_test2("L", "U", "N", "N");
    Rtrsm_test2("L", "U", "T", "U");
    Rtrsm_test2("L", "U", "T", "N");
    Rtrsm_test2("L", "U", "C", "U");
    Rtrsm_test2("L", "U", "C", "N");
    Rtrsm_test2("L", "L", "N", "U");
    Rtrsm_test2("L", "L", "N", "N");
    Rtrsm_test2("L", "L", "T", "U");
    Rtrsm_test2("L", "L", "T", "N");
    Rtrsm_test2("L", "L", "C", "U");
    Rtrsm_test2("L", "L", "C", "N");

    Rtrsm_test2("R", "U", "N", "U");
    Rtrsm_test2("R", "U", "N", "N");
    Rtrsm_test2("R", "U", "T", "U");
    Rtrsm_test2("R", "U", "T", "N");
    Rtrsm_test2("R", "U", "C", "U");
    Rtrsm_test2("R", "U", "C", "N");
    Rtrsm_test2("R", "L", "N", "U");
    Rtrsm_test2("R", "L", "N", "N");
    Rtrsm_test2("R", "L", "T", "U");
    Rtrsm_test2("R", "L", "T", "N");
    Rtrsm_test2("R", "L", "C", "U");
    Rtrsm_test2("R", "L", "C", "N");
}

int main(int argc, char *argv[])
{
    Rtrsm_test();
    printf("Rtrsm test passed...\n");
    return (0);
}
