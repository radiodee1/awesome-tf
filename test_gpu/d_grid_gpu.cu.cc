//#include <math.h>


#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

#include <stdio.h>
#define SIZE	20

    __global__ void VectorAdd( const int *a, int *b, int *c, int n)
    {
        int i = threadIdx.x;

        if (i < n)
            c[i] = a[i] + b[i];
    }

    
    void run(const int * in, int * out) {
        int *a, *b, *c;
        int *d_a, *d_b, *d_c;

        a = (int *)malloc(SIZE*sizeof(int));
        b = (int *)malloc(SIZE*sizeof(int));
        c = (int *)malloc(SIZE*sizeof(int));

        cudaMalloc( &d_a, SIZE*sizeof(int));
        cudaMalloc( &d_b, SIZE*sizeof(int));
        cudaMalloc( &d_c, SIZE*sizeof(int));

        for( int i = 0; i < SIZE; ++i )
        {
            a[i] = i;
            b[i] = i;
            c[i] = 0;
        }

        cudaMemcpy( d_a, a, SIZE*sizeof(int), cudaMemcpyHostToDevice );
        cudaMemcpy( d_b, b, SIZE*sizeof(int), cudaMemcpyHostToDevice );
        cudaMemcpy( d_c, c, SIZE*sizeof(int), cudaMemcpyHostToDevice );

        // blocks, threads
        VectorAdd<<< 1, SIZE >>>(  in, out, d_c, SIZE);
        //VectorAdd<<< 1, SIZE >>>(  d_a, d_b, d_c, SIZE);
                
        cudaMemcpy( c, d_c, SIZE*sizeof(int), cudaMemcpyDeviceToHost );

        for( int i = 0; i < 10; ++i)
            printf("output : c[%d] = %d\n", i, c[i]);

        free(a);
        free(b);
        free(c);

        cudaFree(d_a);
        cudaFree(d_b);
        cudaFree(d_c);
    }


#endif

