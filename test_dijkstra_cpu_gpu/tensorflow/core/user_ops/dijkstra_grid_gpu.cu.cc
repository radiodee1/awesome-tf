#include <math.h>

#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"


#include "dijkstra_grid_gpu.h"
#include "device_functions.h" 

#define VARS_SIGNATURE_DECLARE int * grid_d, int * prev_d, int * mask_d, int * dist_d, int * vars_d, int * lock1_d, int * lock2_d
#define VARS_SIGNATURE_CALL   grid_d,prev_d,mask_d,dist_d,vars_d,lock1_d,lock2_d

    __device__ int get_x(int rank, int * vars_d) { return -1 + rank - (vars_d[SIZEX] * (  (int) ( rank / vars_d[SIZEX] ) )) ; } //-1
    __device__ int get_y(int rank, int * vars_d) { return 0 +  (int) rank / vars_d[SIZEX]  ; } //0
    __device__ int get_rank(int x, int y, int * vars_d) {return 1 + ( (y ) * vars_d[SIZEX] ) + x  ; } // +1
    
    
    
    __device__ void lock(int * mutex) { while (atomicCAS(mutex, 0, 1) != 0  ) ; }
    __device__ void unlock(int * mutex) { atomicExch(mutex, 0); } 
    __device__ void fence(int * mutex, int num) { atomicAdd(mutex, 1) ; while( atomicCAS(mutex, num, num) < num);  }
    
    __device__ float get_a(int test, int rank, int * grid_d) {return (WALL_MULT * sqrt ( 1 + pow(grid_d[test] - grid_d[rank], 2) ));}
    
    __device__ bool wall_found(int test, int rank, VARS_SIGNATURE_DECLARE) {
        
        int a =(int) get_a(test, rank, grid_d);
        
        if ((a >  vars_d[WALLHEIGHT]  * WALL_MULT && grid_d[rank] <= grid_d[test] )|| mask_d[test] == WALL || mask_d[rank] == WALL ) {
            
            return true; 
        }
        return false;
    }
  
    
   
    __device__ bool near_visited( int test, int rank, VARS_SIGNATURE_DECLARE) { 
        
        int size_x = vars_d[SIZEX];
        int size_y = vars_d[SIZEY];
        int start_x = vars_d[STARTX];
        int start_y = vars_d[STARTY];
        
        if(vars_d[FOUND] >= 1) return false;
        
        //lock(&lock2_d[rank]);
        
        if (mask_d[rank] == WALL){
            return false;
        }
        
        if ( test != -1 && test == get_rank( vars_d[STOPX], vars_d[STOPY] , vars_d)  && mask_d[test] == UNDEFINED ) {
                    
            //prev_d[test] = rank ;
            //vars_d[FOUND] =  FOUND_CONST;
            
        }
        
        
        //right
        if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && rank + 1 < size_x * size_y ) {
            if (mask_d[rank + 1] != UNDEFINED && 
                mask_d[rank + 1] != WALL ) return true;
        }
        //left
        if (get_y(rank, vars_d) == get_y(rank - 1, vars_d) && rank - 1 >= 0 ) {
            if (mask_d[rank - 1] != UNDEFINED && 
                mask_d[rank - 1] != WALL) return true;
        }
        //down
        if (rank + size_x < size_x * size_y) {
            if (mask_d[rank + size_x] != UNDEFINED && 
                mask_d[rank + size_x] != WALL ) return true;
        }
        //up
        if (rank - size_x >=0) {
            if (mask_d[rank - size_x] != UNDEFINED && 
                mask_d[rank - size_x ] != WALL) return true;
        }
        
        //unlock(&lock2_d[rank]);
        
            
        if (get_rank(start_x, start_y, vars_d) == rank) {
            
            return true; 
        }
        
        return false; 
    }
  
    __device__ void must_check(int test , int rank, VARS_SIGNATURE_DECLARE) {
    
        

            
        if (   (mask_d[rank] != WALL && mask_d[test] != WALL) ) {
            
            /*
            if ( test == get_rank( vars_d[STOPX], vars_d[STOPY] , vars_d)  && mask_d[test] == UNDEFINED && false ) {
                
                atomicExch(&prev_d[test], rank);
                atomicExch(&vars_d[FOUND], FOUND_CONST);
            }
            */
            
            lock(&lock1_d[test]);
            lock(&lock2_d[rank]);
            
            bool undef = (mask_d[test] == UNDEFINED); //too many zeros
            float a = get_a(test, rank, grid_d);
            int old = dist_d[test];
            int d =  dist_d[rank] + (int) a;
            bool rank_test = true;//(mask_d[rank] != UNDEFINED);
            
            
            //lock(&lock_d[test]); // 
            //if ((d <= dist_d[test] || dist_d[test] == 0   )  && mask_d[test] == UNDEFINED && undef ){
            if (( d <= old || old == 0 ) && undef && rank_test){
                if (! wall_found( test, rank, VARS_SIGNATURE_CALL) ) {
                    
                    atomicExch(&prev_d[test], rank);
                    atomicExch(&dist_d[test], d);
                    
                    
                    //mask_d[test] = vars_d[STEP]; 
                }
            }

            unlock(&lock1_d[test]);
            unlock(&lock2_d[rank]);
        }
        //unlock(&lock_d[test]);
    }
    
    
    
__global__ void DijkstraGridGpu( VARS_SIGNATURE_DECLARE )  {
    
    //start conditions...
    
    mask_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = VISITED;
    prev_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = -1;
    dist_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = 1;
    
    int rank = threadIdx.x + blockIdx.x * blockDim.x;
        
    int left,right,up,down;
    
    int sync = 1;
    
    int size_x = vars_d[SIZEX];
    int size_y = vars_d[SIZEY];
    
    while(  vars_d[FOUND] == 0 && vars_d[STEP] < vars_d[SIZEX] *  vars_d[SIZEY]  +1) {
        
        /*
        left = 0;
        right = 0;
        up = 0;
        down = 0;
        */
        
        //int rank = threadIdx.x + blockIdx.x * blockDim.x;
        
        //vars_d[FENCE2] = 0;
        
        if (rank == 0 || false) vars_d[STEP] ++;
        
        //prev_d[rank] = 1;
        
        //lock(&lock_d[rank]);
        
        //int size_x = vars_d[SIZEX];
        //int size_y = vars_d[SIZEY];
        
        if (rank < vars_d[SIZEX] * vars_d[SIZEY] +1 ){ 
            
        
            if ( false || vars_d[FOUND] == 0) { 
            
                
                if (  mask_d[rank] != WALL  ) { 
                    
                    //right
                    if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && 
                            rank + 1 < size_x * size_y   && near_visited(-1, rank, VARS_SIGNATURE_CALL) ) { 
                        
                        if ( wall_found(rank+1,rank, VARS_SIGNATURE_CALL)) {
                            
                            //mask_d[rank+1 ] = WALL;
                            atomicExch(&mask_d[rank + 1], WALL);
                        }
                        
                    }
                    
                    //left
                    if (get_y(rank, vars_d) == get_y(rank - 1, vars_d) && 
                            rank - 1 >= 0 && near_visited(-1,rank, VARS_SIGNATURE_CALL) ){
                            
                        
                        if ( wall_found(rank -1, rank, VARS_SIGNATURE_CALL)) {
                            
                            
                            //mask_d[rank -1] = WALL;
                            atomicExch(&mask_d[rank - 1], WALL);
                        }
                        
                    }
                    
                    //down
                    if (rank + size_x < size_x * size_y  && near_visited(-1, rank, VARS_SIGNATURE_CALL) ){
                        
                        if( wall_found(rank+ size_x, rank, VARS_SIGNATURE_CALL) ) {
                            
                            //mask_d[rank + size_x] = WALL;
                            atomicExch(&mask_d[rank + size_x], WALL);
                        }
                        
                    }
                    
                    //up
                    if (rank - size_x >=0  && near_visited(-1, rank, VARS_SIGNATURE_CALL) ) { 
                        
                        if ( wall_found(rank - size_x, rank, VARS_SIGNATURE_CALL) ) {
                        
                            
                            //mask_d[rank  - size_x] = WALL;
                            atomicExch(&mask_d[rank - size_x], WALL);
                        }
                        
                    }
                    
                    if (near_visited(-1, rank, VARS_SIGNATURE_CALL) ){
                        
                        //lock(&lock_d[rank]);
                        if (false || ( mask_d[rank] == UNDEFINED && mask_d[rank] != WALL)) {
                            //mask_d[rank] = vars_d[STEP];
                            atomicExch(&mask_d[rank ], vars_d[STEP]);
                        }
                        //unlock(&lock_d[rank]);
                    }
                    /////////////////////////////
                }// if 1
            }// if 2
        }//if 3
    }//while
    
    fence(&vars_d[FENCE1], (size_x * size_y ) );//* (vars_d[STEP]) );
    vars_d[STEP] = 0;
    
    if (mask_d[rank] != WALL) mask_d[rank] = UNDEFINED;  
                    
    while( vars_d[FOUND] == 0 && vars_d[STEP] < vars_d[SIZEX] *  vars_d[SIZEY]  +1) {
        
        left = 0;
        right = 0;
        up = 0;
        down = 0;
        
        //vars_d[FENCE2] = 0;
        
        if (rank == 0 || false) vars_d[STEP] ++;
        
        
        if (rank < vars_d[SIZEX] * vars_d[SIZEY] + 1 ){ 
            
        
            if ( false || vars_d[FOUND] == 0) { 
            
                
                if ( ( mask_d[rank] != WALL ) ){ // && mask_d[rank] != UNDEFINED ) { 
                    
                    /////////////////////////////
                    //if (sync == 1) __syncthreads();

                    //fence(&vars_d[FENCE1], (size_x * size_y ) * (vars_d[STEP]) );
                    //if (rank == 0) atomicExch(&vars_d[FENCE1], 0);
                    
                    //right
                    if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && 
                            rank + 1 < size_x * size_y && near_visited(- 1, rank, VARS_SIGNATURE_CALL) ) { 
                        
                        if (! wall_found(rank+1,rank, VARS_SIGNATURE_CALL)) {
                            
                            right = 1;
                            must_check(rank + 1, rank, VARS_SIGNATURE_CALL) ;
                        }
                        
                    }
                    
                    //left
                    if (get_y(rank, vars_d) == get_y(rank - 1, vars_d) && 
                            rank - 1 >= 0 && near_visited(-1, rank, VARS_SIGNATURE_CALL) ){
                            
                        
                        if (! wall_found(rank -1, rank, VARS_SIGNATURE_CALL)) {
                            
                            left = 1;
                            must_check(rank -1, rank, VARS_SIGNATURE_CALL);
                        }
                        
                    }
                    
                    //down
                    if (rank + size_x < size_x * size_y && near_visited(-1, rank, VARS_SIGNATURE_CALL) ){
                        
                        if(! wall_found(rank+ size_x, rank, VARS_SIGNATURE_CALL) ) {
                            
                            down = 1;
                            must_check(rank + size_x, rank, VARS_SIGNATURE_CALL);
                        }
                        
                    }
                    
                    //up
                    if (rank - size_x >=0 && near_visited(-1, rank, VARS_SIGNATURE_CALL) ) { 
                        
                        if (! wall_found(rank - size_x, rank, VARS_SIGNATURE_CALL) ) {
                        
                            up = 1;
                            must_check(rank - size_x, rank, VARS_SIGNATURE_CALL);
                        }
                        
                    }
                    /////////////////////////
                    
                    /////////////////////////
                    if (sync == 1) __syncthreads();
                    
                    //fence(&vars_d[FENCE2], (size_x * size_y) * (vars_d[STEP] ) );
                    //if (rank == 0) atomicExch(&vars_d[FENCE2], 0);
                    //lock(&lock_d[rank]); // 
                    if(right == 1) {
                        //must_check(rank + 1, rank, VARS_SIGNATURE_CALL) ;
                    }
                    if(up == 1) {
                        //must_check(rank - size_x, rank, VARS_SIGNATURE_CALL);
                    }
                    if(left == 1) {
                        //must_check(rank -1, rank, VARS_SIGNATURE_CALL);
                    }
                    if(down == 1) {
                        //must_check(rank + size_x, rank, VARS_SIGNATURE_CALL);
                    }
                    
                    //lock(&lock_d[rank]);
                    ////////////////////////
                    //fence(&vars_d[FENCE3], (size_x * size_y) * (vars_d[STEP] ) );
                    
                    if (near_visited(-1, rank, VARS_SIGNATURE_CALL) ){
                        
                        //lock(&lock_d[rank]);
                        if (false || ( mask_d[rank] == UNDEFINED && mask_d[rank] != WALL)) {
                            //mask_d[rank] = vars_d[STEP];
                            atomicExch(&mask_d[rank ], vars_d[STEP]);
                        }
                        //unlock(&lock_d[rank]);
                    }
                    //unlock(&lock_d[rank]); // 
                    if ( rank == get_rank( vars_d[STOPX], vars_d[STOPY] , vars_d) && near_visited(-1, rank, VARS_SIGNATURE_CALL)  && 
                            //mask_d[rank] == UNDEFINED && 
                            mask_d[rank] != WALL) {
                        if ( true ) {
                            //vars_d[FOUND] = FOUND_CONST;

                            atomicExch(&vars_d[FOUND], FOUND_CONST);
                        }
                        
                    }
                    

                    
                }

            }

        } // if
        
        //unlock(&lock_d[rank]);

    } // while
    
    
    
    //vars_d[FOUND] = blockDim.x;
    
  }
  

    void DijkstraGridGpuLauncher(int size_x, int size_y, int * grid, int * prev, int * mask, int * dist, int * vars ) {
        
        int * grid_d, * prev_d, * mask_d, * dist_d, * vars_d, * lock1_d, * lock2_d;
    
        int size = size_x * size_y;
        int SIZE = 512;//1024;
        int blocks =  size/SIZE ;
        if (blocks == 0) blocks = 1;
        int threads = SIZE;
        if (size < SIZE || false) threads = size;
        
        cudaMalloc( &grid_d, size*sizeof(int));
        cudaMalloc( &prev_d, size*sizeof(int));
        cudaMalloc( &mask_d, size*sizeof(int));
        cudaMalloc( &dist_d, size*sizeof(int));
        cudaMalloc( &vars_d, VARS_ARRAY_SIZE*sizeof(int));
        cudaMalloc( &lock1_d, size*sizeof(int));
        cudaMalloc( &lock2_d, size*sizeof(int));

        cudaMemcpy( grid_d, grid, size*sizeof(int), cudaMemcpyHostToDevice );
        //cudaMemcpy( prev_d, prev, size*sizeof(int), cudaMemcpyHostToDevice );
        //cudaMemcpy( mask_d, mask, size*sizeof(int), cudaMemcpyHostToDevice );
        //cudaMemcpy( dist_d, dist, size*sizeof(int), cudaMemcpyHostToDevice );
        cudaMemcpy( vars_d, vars, VARS_ARRAY_SIZE*sizeof(int), cudaMemcpyHostToDevice );
        
        cudaMemset(mask_d, 0, size*sizeof(int));
        cudaMemset(dist_d, 0, size*sizeof(int));
        cudaMemset(prev_d, 0, size*sizeof(int));
        cudaMemset(lock1_d, 0, size*sizeof(int));
        cudaMemset(lock2_d, 0, size*sizeof(int));
        
        // 1 block, size_x*size_y threads
        DijkstraGridGpu  <<< blocks, threads >>>( grid_d, prev_d, mask_d, dist_d, vars_d, lock1_d, lock2_d);
        
        
        cudaMemcpy( prev, prev_d, size*sizeof(int), cudaMemcpyDeviceToHost );
        cudaMemcpy( vars, vars_d, VARS_ARRAY_SIZE*sizeof(int), cudaMemcpyDeviceToHost );
        cudaMemcpy( mask, mask_d, size*sizeof(int), cudaMemcpyDeviceToHost );
        cudaMemcpy( dist, dist_d, size*sizeof(int), cudaMemcpyDeviceToHost );
        
        printf("vars block %i threads %i \n", blocks,threads);
        printf("vars start x %i \n", vars[0]);
        printf("vars start y %i \n", vars[1]);
        printf("vars stop x %i \n", vars[2]);
        printf("vars stop y %i \n", vars[3]);
        printf("vars size x %i \n", vars[4]);
        printf("vars size y %i \n", vars[5]);
        printf("vars wallheight %i \n", vars[6]);
        printf("vars found %i \n", vars[7]);
        printf("vars step %i \n", vars[8]);
        printf("vars fence1 %i \n", vars[FENCE1]);
        printf("vars fence2 %i \n", vars[FENCE2]);
        printf("start spot %i \n", mask[size_x * vars[STARTY] + vars[STARTX]  ]);
        
        for (int i = 0; i < size; i ++) {
            printf(",%i ", prev[i]);
        }
        printf(" size %i \n",size);
        

        
        //free(grid);
        //free(mask);
        //free(dist);
        //free(vars);
        
        cudaFree(grid_d);
        cudaFree(prev_d);
        cudaFree(mask_d);
        cudaFree(dist_d);
        cudaFree(vars_d);
        cudaFree(lock1_d);
        cudaFree(lock2_d);
    }



#endif

