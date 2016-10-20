#include <cmath>

#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

#include "tensorflow/core/framework/op.h"
#include "dijkstra_grid_gpu.h"

#define WALL -1
#define VISITED 1
#define UNDEFINED 0

#define WALL_MULT 100

#define VARS_SIGNATURE_DECLARE int * grid_d, int * prev_d, int * mask_d, int * dist_d, int * vars_d
#define VARS_SIGNATURE_CALL   grid_d,prev_d,mask_d,dist_d,vars_d

#include "tensorflow/core/framework/op_kernel.h"


using namespace tensorflow;

    //signatures...

    __device__ int get_x(int rank, int * vars_d) ;
    __device__ int get_y(int rank, int * vars_d) ;
    __device__ int get_rank(int x, int y, int * vars_d) ;
    __device__ bool near_visited( int rank, VARS_SIGNATURE_DECLARE) ;
    __device__ void must_check(int test , int rank, VARS_SIGNATURE_DECLARE) ;
    __device__ bool wall_found(int test, int rank, VARS_SIGNATURE_DECLARE) ;
    
    //entry point...
    
__global__ void DijkstraGridGpu(VARS_SIGNATURE_DECLARE )  {
    
    
    
    //start conditions...
    
    mask_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = VISITED;
    prev_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = -1;
    dist_d[get_rank( vars_d[STARTX], vars_d[STARTY], vars_d) ] = 1;

    
    while(  vars_d[FOUND] && vars_d[STEP] < vars_d[SIZEX] * vars_d[SIZEY] +1) {
        
        vars_d[STEP] ++;
        
        
        int rank = threadIdx.x;
        
        int size_x = vars_d[SIZEX];
        int size_y = vars_d[SIZEY];
        
        if (rank < vars_d[SIZEX] * vars_d[SIZEY] ){//  size_x * size_y) {

        
            if ( vars_d[FOUND] != 1) { 
            
                
                if (  mask_d[rank] != WALL  ) { 
                    //right
                    if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && 
                            rank + 1 < size_x * size_y && near_visited( rank, VARS_SIGNATURE_CALL) ) { 
                        //std::cout << "right\n";
                        if (! wall_found(rank+1,rank, VARS_SIGNATURE_CALL)) {
                            
                            
                            must_check(rank + 1, rank, VARS_SIGNATURE_CALL);
                            //break;
                        }
                        else {
                            mask_d[rank+1 ] = WALL;
                        }
                    }
                    
                    //left
                    if (get_y(rank, vars_d) == get_y(rank - 1, vars_d) && 
                            rank - 1 >= 0 && near_visited(rank, VARS_SIGNATURE_CALL) ){
                            
                        //std::cout << "left\n";
                        if (! wall_found(rank -1, rank, VARS_SIGNATURE_CALL)) {
                            
                            
                            must_check(rank -1, rank, VARS_SIGNATURE_CALL);
                            //break;
                        }
                        else {
                            mask_d[rank -1] = WALL;
                        }
                    }
                    //down
                    if (rank + size_x < size_x * size_y && near_visited(rank, VARS_SIGNATURE_CALL) ){
                        //std::cout << "down\n";
                        if(! wall_found(rank+ size_x, rank, VARS_SIGNATURE_CALL) ) {
                            
                            
                            must_check(rank + size_x, rank, VARS_SIGNATURE_CALL);
                            //break;
                        }
                        else {
                            mask_d[rank + size_x] = WALL;
                        }
                    }
                    //up
                    if (rank - size_x >=0 && near_visited(rank, VARS_SIGNATURE_CALL) ) { 
                        //std::cout << "up\n";
                        if (! wall_found(rank - size_x, rank, VARS_SIGNATURE_CALL) ) {
                        
                            
                            must_check(rank - size_x, rank, VARS_SIGNATURE_CALL);
                            //break;
                        }
                        else {
                            mask_d[rank  - size_x] = WALL;
                        }
                    }
                    
                    if (near_visited(rank, VARS_SIGNATURE_CALL) ){
                        
                        if (true || mask_d[rank] == UNDEFINED) mask_d[rank] = vars_d[STEP];
                    }
                    if ( rank == get_rank( vars_d[STOPX], vars_d[STOPY] , vars_d) && mask_d[rank] != UNDEFINED ) {
                        //found = true;
                        vars_d[FOUND] = 1;
                        
                        //std::cout << "stop here !!\n" ;
                    }
                    
                    
                }
            
            }

        } // if
    } // while

    
    //std::cout << "loop = " << step << ", wall = " << wall_height << "\n";
    
    //print();
    
    
  }
  
  
    
    __device__ int get_x(int rank, int * vars_d) { return -1 + rank - (vars_d[SIZEX] * (  (int) ( rank / vars_d[SIZEX] ) )) ; }
    __device__ int get_y(int rank, int * vars_d) { return 0 +  (int) rank / vars_d[SIZEX]  ; }
    __device__ int get_rank(int x, int y, int * vars_d) {return 1+ ( (y ) * vars_d[SIZEX] ) + x  ; }
    
    __device__ bool near_visited( int rank, VARS_SIGNATURE_DECLARE) { 
        
        int size_x = vars_d[SIZEX];
        int size_y = vars_d[SIZEY];
        int start_x = vars_d[STARTX];
        int start_y = vars_d[STARTY];
        
        if(vars_d[FOUND]) return false;
        
        if (mask_d[rank] == WALL){
            return false;
        }
        //right
        if (get_y(rank, vars_d) == get_y(rank + 1, vars_d) && rank + 1 <= size_x * size_y ) {
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
        if (get_rank(start_x, start_y, vars_d) == rank) {
            //std::cout << "start!\n";
            return true; 
        }
        
        return false; 
    }
  
    __device__ void must_check(int test , int rank, VARS_SIGNATURE_DECLARE) {
    
        int start_x = vars_d[STARTX];
        int start_y = vars_d[STARTY];
        
        
        float a = WALL_MULT * sqrt ( 1 + pow(grid_d[test] - grid_d[rank], 2) );
        int d =  dist_d[rank] + (int) a;
        
        if (  ! wall_found(test,rank, VARS_SIGNATURE_CALL) && mask_d[rank] != WALL && mask_d[test] != WALL) {
            
            
            if ((d < dist_d[test] || dist_d[test] == 0)   ){
                if (true || get_x(rank, vars_d) != start_x || get_y(rank, vars_d) != start_y) {
                    
                    
                    prev_d[test] = rank ;
                    dist_d[test] = d;
                    
                }
                
            
            }
        }
        
    }
    
    __device__ bool wall_found(int test, int rank, VARS_SIGNATURE_DECLARE) {
    
        //auto mask = mask_tensor.template flat<int32>();
        //auto grid = input_tensor.template flat<int32>();
        
        float a = WALL_MULT * sqrt ( 1 + pow(grid_d[test] - grid_d[rank], 2) );
        if ((a >  vars_d[WALLHEIGHT]  * WALL_MULT && grid_d[rank] <= grid_d[test] )|| mask_d[test] == WALL  ) {
            //std::cout << test << " " << rank << " " << a << " -- wall!!\n";
            return true; 
        }
        return false;
    }
    
    /*
    void print() {
        auto mask = input_tensor.template flat<int32>();

        for (int rank = 0; rank < mask.size(); rank++) {
            std::cout << mask_d[rank] << ",";
            if (rank % size_x == size_x - 1) std::cout << "\n";
        }
        std::cout << "\n";

        
    }
    */
    
//};

    void DijkstraGridGpuLauncher(int size_x, int size_y, int * grid_d, int * prev_d, int * mask_d, int * dist_d, int * vars_d ) {
                                
        DijkstraGridGpu  <<< 1, size_x * size_y >>>( VARS_SIGNATURE_CALL);
    }

#endif

