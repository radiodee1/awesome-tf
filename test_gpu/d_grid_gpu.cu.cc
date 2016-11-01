
#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

#include <stdio.h>
#define SIZE	20

    __global__ void VectorAdd(  int *in,  int n)
    {
        int i = threadIdx.x;

        if (i < n)
            in[i] = in[i] + i;
    }

    
    void run( int * in){//, int * out) {
        
        VectorAdd<<< 1, SIZE >>>(  in,  SIZE);
        
        for (int i = 0; i < SIZE; i ++) {
            printf("%i, " , in[i]);
        }
    }


#endif

