#include <math.h>
#include <sys/time.h>

#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"


#include "dijkstra_grid_gpu.h"
#include "device_functions.h" 

#define VARS_SIGNATURE_DECLARE int * grid_d, int * prev_d, int * mask_d, int * dist_d, int * vars_d 

#define VARS_SIGNATURE_CALL   grid_d,prev_d,mask_d,dist_d,vars_d 


    __device__ int get_x(int rank, int * vars_d) { return 0 + rank - (vars_d[SIZEX] * (  (int) ( rank / vars_d[SIZEX] ) )) ; } //-1
    __device__ int get_y(int rank, int * vars_d) { return 0 +  (int) rank / vars_d[SIZEX]  ; } 
    __device__ int get_rank(int x, int y, int * vars_d) {return abs( 0 + ( (y ) * vars_d[SIZEX] ) + x )  ; } // +1
    __device__ bool is_filled(int * mask, int size) {bool value = true; for(int i = 0; i < size; i ++) {if( mask[i] == UNDEFINED) value = false; } ; return value;}
    
    
    __device__ void lock(int * mutex) { while (atomicCAS(mutex, 0, 1) != 0  ) ; } 
    __device__ void unlock(int * mutex) { atomicExch(mutex, 0); } 
    __device__ void fence(int * mutex, int num) { atomicAdd(mutex, 1) ; while( atomicCAS(mutex, num, num) < num);  }
    
    __device__ float get_a(int test, int rank, int * grid_d) {return (WALL_MULT * sqrt ( 1 + pow(grid_d[test] - grid_d[rank], 2) ));}
    
    __device__ bool wall_found(int test, int rank, VARS_SIGNATURE_DECLARE) {
        
        int a =(int) get_a(test, rank, grid_d);
        
        if ((a >  vars_d[WALLHEIGHT]   * WALL_MULT   && grid_d[rank] <= grid_d[test] )|| mask_d[test] == WALL || mask_d[rank] == WALL ) {
            
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
        
        if (mask_d[rank] == WALL){
            return false;
        }
        
        //right
        if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && rank + 1 < size_x * size_y ) {
            if (mask_d[rank + 1] != UNDEFINED && 
                mask_d[rank + 1] != WORKING && 
                mask_d[rank + 1] != WALL ) return true;
        }
        //left
        if (get_y(rank, vars_d) == get_y(rank - 1, vars_d) && rank - 1 >= 0 ) {
            if (mask_d[rank - 1] != UNDEFINED &&
                mask_d[rank - 1] != WORKING &&  
                mask_d[rank - 1] != WALL) return true;
        }
        //down
        if (rank + size_x < size_x * size_y) {
            if (mask_d[rank + size_x] != UNDEFINED &&
                mask_d[rank + size_x] != WORKING &&  
                mask_d[rank + size_x] != WALL ) return true;
        }
        //up
        if (rank - size_x >=0) {
            if (mask_d[rank - size_x] != UNDEFINED &&
                mask_d[rank - size_x] != WORKING &&  
                mask_d[rank - size_x ] != WALL) return true;
        }
        
            
        if (get_rank(start_x, start_y, vars_d) == rank) {
            
            return true; 
        }
        
        return false; 
    }
  
    __device__ void must_check(int test , int rank, VARS_SIGNATURE_DECLARE) {
    
        
        
        if ( true ||  (mask_d[rank] != WALL && mask_d[test] != WALL) ) {
            
            
            bool undef = (mask_d[test] != WALL); 
            float a = get_a(test, rank, grid_d);
            int old = dist_d[test];
            int d =  dist_d[rank] + (int) a;
            bool rank_mask = true;//(mask_d[rank] == WORKING);
            
            
            
            if (( d <= old || old == 0 ) && undef && rank_mask){
                if ( true ) {
                    
                    atomicExch(&prev_d[test], rank);
                    atomicExch(&dist_d[test], d);
                    
                    
                    //mask_d[test] = vars_d[STEP]; 
                }
            }

            

        }
        
    }
    
    
    
__global__ void DijkstraGridGpu( VARS_SIGNATURE_DECLARE )  {
    
    //start conditions...
    
    mask_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = VISITED;
    prev_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = -1;
    dist_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = 1;
    
    int rank = threadIdx.x + blockIdx.x * blockDim.x;
        
    int left,right,up,down;
    
    //int sync = 1;
    
    int size_x = vars_d[SIZEX];
    int size_y = vars_d[SIZEY];
    
    while(  vars_d[FOUND] == 0 && vars_d[STEP] < vars_d[SIZEX] *  vars_d[SIZEY]  + 1) {
        
        
        
        if (rank == 0 ) vars_d[STEP] ++;
        
        
        
        if (rank < vars_d[SIZEX] * vars_d[SIZEY]  ){ 
            
        
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
                        if (false || ( mask_d[rank] == UNDEFINED && mask_d[rank] != WALL) ){// || is_filled(mask_d, size_x * size_y)) {
                            
                            atomicExch(&mask_d[rank ], vars_d[STEP]);
                        }
                        
                    }
                    /////////////////////////////
                }// if 1
            }// if 2
        }//if 3
    }//while
    
    if (rank < size_x * size_y) fence(&vars_d[FENCE1], (size_x * size_y ) );
    vars_d[STEP] = 0;
    
    if (mask_d[rank] != WALL) mask_d[rank] = UNDEFINED;  
    
    mask_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = VISITED;
    prev_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = -1;
    dist_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = 1;
    
    int count;
                    
    while( vars_d[FOUND] == 0 && vars_d[STEP] < vars_d[SIZEX] *  vars_d[SIZEY] * vars_d[SIZEX]  ) {
        
        
        left = 0;
        right = 0;
        up = 0;
        down = 0;
        
        
        count = 0;
        
        if (rank == 0 || false) vars_d[STEP] ++;
        
        
        if (rank < vars_d[SIZEX] * vars_d[SIZEY]  ){ 
            
        
            if ( false || vars_d[FOUND] == 0) { 
            
                
                if ( ( mask_d[rank] != WALL && mask_d[rank] == UNDEFINED   && 
                    near_visited(-1, rank, VARS_SIGNATURE_CALL) ) ||
                    rank == get_rank(vars_d[STARTX], vars_d[STARTY], vars_d) ){ 
                    
                    /////////////////////////////


                    if (true ){
                        

                        if (true){// || ( 
                            //mask_d[rank] == UNDEFINED && 
                            //mask_d[rank] != WALL)) {
                            
                            atomicExch(&mask_d[rank ], WORKING);

                        }

                    }

                    while (count < 4 && vars_d[FOUND] < 1 ) {
                        //right
                        if (right == 0) {
                            if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && 
                                    rank + 1 < size_x * size_y && 
                                    near_visited(- 1, rank, VARS_SIGNATURE_CALL) &&
                                    ! wall_found(rank +1, rank, VARS_SIGNATURE_CALL) 
                                    ) { 
                                
                                if (true){// lock2_d[rank + 1] == 0){
                                    
                                    //right = 1;
                                    must_check(rank + 1, rank, VARS_SIGNATURE_CALL) ;
                                    //count ++;
                                    right = 1;
                                }
                                
                            }
                            else {
                                //count ++;
                                right = 1;
                            }
                        }
                        
                        //left
                        if (left == 0) {
                            if (get_y(rank, vars_d) == get_y(rank - 1, vars_d) && 
                                    rank - 1 >= 0 && 
                                    near_visited(-1, rank, VARS_SIGNATURE_CALL) &&
                                    ! wall_found(rank - 1, rank, VARS_SIGNATURE_CALL) ){
                                    
                                
                                if (true ){//lock2_d[rank - 1] == 0 ){
                                    
                                    //left = 1;
                                    must_check(rank -1, rank, VARS_SIGNATURE_CALL);
                                    //count ++;
                                    left = 1;
                                }
                                
                            }
                            else {
                                //count ++;
                                left = 1;
                            }
                        }
                        
                        //down
                        if (down == 0) {
                            if (rank + size_x < size_x * size_y && 
                                    near_visited(-1, rank, VARS_SIGNATURE_CALL) &&
                                    ! wall_found(rank + size_x, rank, VARS_SIGNATURE_CALL) ){
                                
                                if(true ){//lock2_d[rank + size_x] == 0 ){
                                    
                                    //down = 1;
                                    must_check(rank + size_x, rank, VARS_SIGNATURE_CALL);
                                    //count ++;
                                    down = 1;
                                }
                                
                            }
                            else {
                                //count ++;
                                down = 1;
                            }
                        }
                        
                        //up
                        if (up == 0) {
                            if (rank - size_x >=0 && 
                                    near_visited(-1, rank, VARS_SIGNATURE_CALL) &&
                                    ! wall_found(rank - size_x, rank, VARS_SIGNATURE_CALL) ) { 
                                
                                if ( true){// lock2_d[rank - size_x] == 0 ){
                                
                                    //up = 1;
                                    must_check(rank - size_x, rank, VARS_SIGNATURE_CALL);
                                    //count ++;
                                    up = 1;
                                }
                                
                            }
                            else {
                                //count ++;
                                up = 1;
                            }
                        }
                        count = left + right + up + down;
                    }
                    /////////////////////////
                    
                    
                    if ( vars_d[STOPX] >= 0 && vars_d[STOPY] >= 0 && 
                        (rank == get_rank( vars_d[STOPX], vars_d[STOPY] , vars_d) && 
                            near_visited(-1, rank, VARS_SIGNATURE_CALL)  && 
                            (mask_d[rank] == UNDEFINED || mask_d[rank] == WORKING) && 
                            mask_d[rank] != WALL) ){
                            
                        if ( true ) {

                            atomicExch(&mask_d[rank ], VISITED);
                            atomicExch(&vars_d[FOUND], FOUND_CONST);
                            
                            vars_d[FENCE2] = get_x(rank, vars_d);
                            vars_d[FENCE3] = get_y(rank, vars_d);
                        }
                        
                    }
                    
                    if (near_visited(-1, rank, VARS_SIGNATURE_CALL) || true ){
                        

                        if (false || ( 
                            mask_d[rank] == WORKING //&& 
                            //mask_d[rank] != WALL
                            )) {
                            
                            atomicExch(&mask_d[rank ], VISITED);

                        }

                    }
                    
                    
                }
                
            }

        } // if
        


    } // while
    
    
    
    //vars_d[FOUND] = blockDim.x;
    
  }
  
    void showVariable(int * device, int size ) {
        int size_x = 8;
        int * host;
        host = (int *) malloc(  size*sizeof(int));
        cudaMemcpy( host, device, size*sizeof(int), cudaMemcpyDeviceToHost );
        for (int i = 0; i < size; i ++) {
            printf(",%i ", host[i]);
            if (i % size_x == size_x -1) printf("\n");
        }
        printf(" size %i \n",size);
        free(host);
    }
  
    void DijkstraGridGpuLauncherTF(int size_x, int size_y, int * grid, int * prev, int * mask, int * dist, int * vars , int * vars_host) {
    
        int size = size_x * size_y;
        
        cudaMemcpy( vars, vars_host, VARS_ARRAY_SIZE*sizeof(int), cudaMemcpyHostToDevice );
        
        cudaMemset(mask, 0, size*sizeof(int));
        cudaMemset(dist, 0, size*sizeof(int));
        cudaMemset(prev, 0, size*sizeof(int)); 
        
        
        int SIZE = 1024;
        int blocks =  size/SIZE +1;

        int threads = SIZE;
        if (blocks == 1 && size < SIZE ) threads = size;
        
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        DijkstraGridGpu  <<< blocks, threads >>>( grid, prev, mask, dist, vars );
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        float delta_us = (float) (end.tv_nsec - start.tv_nsec)   / 1000;

        
        printf("time elapsed on gpu %f \n---\n", delta_us);
        
        //showVariable(prev, size);
        //showVariable(mask, size);
        //showVariable(vars,VARS_ARRAY_SIZE);
        
        
    }

    /*
    void DijkstraGridGpuLauncher(int size_x, int size_y, int * grid, int * prev, int * mask, int * dist, int * vars ) {
        
        int * grid_d, * prev_d, * mask_d, * dist_d, * vars_d;
    
        int size = size_x * size_y;
        int SIZE = 1024;
        int blocks =  size/SIZE +1;

        int threads = SIZE;
        if (blocks == 1 && size < SIZE ) threads = size;
        
        cudaMalloc( &grid_d, size*sizeof(int));
        cudaMalloc( &prev_d, size*sizeof(int));
        cudaMalloc( &mask_d, size*sizeof(int));
        cudaMalloc( &dist_d, size*sizeof(int));
        cudaMalloc( &vars_d, VARS_ARRAY_SIZE*sizeof(int));
        
        cudaMemcpy( grid_d, grid, size*sizeof(int), cudaMemcpyHostToDevice );
        
        cudaMemcpy( vars_d, vars, VARS_ARRAY_SIZE*sizeof(int), cudaMemcpyHostToDevice );
        
        cudaMemset(mask_d, 0, size*sizeof(int));
        cudaMemset(dist_d, 0, size*sizeof(int));
        cudaMemset(prev_d, 0, size*sizeof(int));


        
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        DijkstraGridGpu  <<< blocks, threads >>>( grid_d, prev_d, mask_d, dist_d, vars_d );
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        float delta_us = (float) (end.tv_nsec - start.tv_nsec)   / 1000;

        
        cudaMemcpy( prev, prev_d, size*sizeof(int), cudaMemcpyDeviceToHost );
        cudaMemcpy( vars, vars_d, VARS_ARRAY_SIZE*sizeof(int), cudaMemcpyDeviceToHost );
        cudaMemcpy( mask, mask_d, size*sizeof(int), cudaMemcpyDeviceToHost );
        cudaMemcpy( dist, dist_d, size*sizeof(int), cudaMemcpyDeviceToHost );
        
        
        
        cudaFree(grid_d);
        cudaFree(prev_d);
        cudaFree(mask_d);
        cudaFree(dist_d);
        cudaFree(vars_d);
        
    }
    */



#endif

