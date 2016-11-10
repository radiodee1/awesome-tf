
#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

#include <stdio.h>
#define SIZE	10

    __global__ void VectorAdd(const  int *in, int * out, int n)
    {
        int i = threadIdx.x;

        if (i < n)
            out[i] = in[i] + i;
    }

    
    void run( const int * in, int * out) {
        
        VectorAdd<<< 1, SIZE >>>(  in, out,  SIZE);
        
        /*
        //these lines cause the segfault
        for (int i = 0; i < SIZE; i ++) {
            printf("%i, " , out[i]);
        }
        */
    }


#endif

