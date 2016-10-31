
#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

#include <stdio.h>
#define SIZE	20

    __global__ void VectorAdd( const int *in, int *out,  int n)
    {
        int i = threadIdx.x;

        if (i < n)
            out[i] = in[i] + out[i];
    }

    
    void run(const int * in, int * out) {
        
        VectorAdd<<< 1, SIZE >>>(  in, out, SIZE);
        
    }


#endif

