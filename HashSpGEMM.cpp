#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <fstream>
#include <iterator>
#include <ctime>
#include <cmath>
#include <string>
#include <sstream>
#include <random>
#include "utility.h"
#include "CSR.h"
#include "multiply.h"
#include "hash_mult.h"
#include "sample_common.hpp"

using namespace std;

#define VALUETYPE double
#define INDEXTYPE int

int main(int argc, char* argv[])
{
    const bool sortOutput = true;
    vector<int> tnums;
	CSR<INDEXTYPE,VALUETYPE> A_csr, B_csr; // Initailize input csr objects using csr header.
#ifdef KNL_EXE
        // if it is intel KNL processor.
        cout << "Running on 1,1,2,2 threads" << endl << endl;
        tnums = {1,1,2,2};
        // tnums = {1, 2, 4, 8, 16, 32, 64, 68, 128, 136, 192, 204, 256, 272}; // for scalability test
#else
        cout << "Running on 1,2 threads" << endl;
        tnums = {1,2}; // for hashwell intel processor according to the arcticle.
#endif

    /* Generating input matrices based on argument */
    //linked to sample_common.hpp header.
    SetInputMatricesAsCSR(A_csr, B_csr, argv);

  	A_csr.Sorted();
  	B_csr.Sorted();

    /* Count total number of floating-point operations */
    long long int nfop = get_flop(A_csr, B_csr); // returns off set sum or (total calculations of matrix multiplication).
    cout << "Total number of floating-point operations including addition and multiplication in SpGEMM (A * B): " << nfop << endl << endl;

    double start, end, msec, ave_msec, mflops;

    /* Execute Hash-SpGEMM */
    cout << "Evaluation of HashSpGEMM" << endl;
    for (int tnum : tnums) {
        omp_set_num_threads(tnum);

        CSR<INDEXTYPE,VALUETYPE> C_csr;

        /* First execution is excluded from evaluation */
        HashSpGEMM<false, sortOutput>(A_csr, B_csr, C_csr, multiplies<VALUETYPE>(), plus<VALUETYPE>()); //function linked to hash_mult.h header. For computing matrix multiplication.
        C_csr.make_empty();

        ave_msec = 0;
        for (int i = 0; i < ITERS; ++i) {
            start = omp_get_wtime();
            HashSpGEMM<false, sortOutput>(A_csr, B_csr, C_csr, multiplies<VALUETYPE>(), plus<VALUETYPE>());
            end = omp_get_wtime();
            msec = (end - start) * 1000;
            ave_msec += msec;
            if (i < ITERS - 1) {
                C_csr.make_empty();
            }
        }
        ave_msec /= ITERS;
        mflops = (double)nfop / ave_msec / 1000;
        printf("HashSpGEMM returned with %d nonzeros. Compression ratio is %f\n", C_csr.nnz, (float)(nfop / 2) / (float)(C_csr.nnz));
        printf("HashSpGEMM with %3d threads computes C = A * B in %f [milli seconds] (%f [MFLOPS])\n\n", tnum, ave_msec, mflops);
        C_csr.make_empty();
    }
    A_csr.make_empty();
    B_csr.make_empty();
    return 0;
}
