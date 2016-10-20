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


#include "tensorflow/core/framework/op_kernel.h"


using namespace tensorflow;

//class DijkstraGridOp : public OpKernel {
//  public:
  

    __global__ void DijkstraGridGpu(int * grid_d, int * prev_d, int * mask_d, int * dist_d, 
                            int * vars_d)  {
    
    //const int N = grid.size();
    //step = 0;
    
    //for (int rank = 0; rank < N; rank++) prev_d[rank] = UNDEFINED;
    mask_d[get_rank(start_x, start_y)] = VISITED;
    prev_d[get_rank(start_x, start_y)] = -1;
    dist_d[get_rank(start_x, start_y)] = 1;

    
    while( !found && step < size_x * size_y +1) {
        //step ++;
        vars_d[STEP] ++;
        
        //std::cout << step << " = while loop i \n";
        //print();
        rank = threadIdx.x;
        
        //for (int rank = 0; rank < N; rank++) {
        if (rank < vars_d[SIZEX] * vars_d[SIZEY] ){//  size_x * size_y) {

        
            if ( vars_d[FOUND] != 1) { 
            
                
                if (  mask_d[rank] != WALL  ) { 
                    //right
                    if (get_y(rank) == get_y(rank + 1) && 
                            rank + 1 < size_x * size_y && near_visited( rank) ) { 
                        //std::cout << "right\n";
                        if (! wall_found(rank+1,rank)) {
                            
                            
                            must_check(rank + 1, rank);
                            //break;
                        }
                        else {
                            mask_d[rank+1 ] = WALL;
                        }
                    }
                    
                    //left
                    if (get_y(rank) == get_y(rank - 1) && 
                            rank - 1 >= 0 && near_visited(rank) ){
                            
                        //std::cout << "left\n";
                        if (! wall_found(rank -1, rank)) {
                            
                            
                            must_check(rank -1, rank);
                            //break;
                        }
                        else {
                            mask_d[rank -1] = WALL;
                        }
                    }
                    //down
                    if (rank + size_x < size_x * size_y && near_visited(rank) ){
                        //std::cout << "down\n";
                        if(! wall_found(rank+ size_x, rank) ) {
                            
                            
                            must_check(rank + size_x, rank);
                            //break;
                        }
                        else {
                            mask_d[rank + size_x] = WALL;
                        }
                    }
                    //up
                    if (rank - size_x >=0 && near_visited(rank) ) { 
                        //std::cout << "up\n";
                        if (! wall_found(rank - size_x, rank) ) {
                        
                            
                            must_check(rank - size_x, rank);
                            //break;
                        }
                        else {
                            mask_d[rank  - size_x] = WALL;
                        }
                    }
                    
                    if (near_visited(rank) ){
                        
                        if (true || mask_d[rank] == UNDEFINED) mask_d[rank] = vars_d[STEP];
                    }
                    if ( rank == get_rank( vars_d[STOPX], vars_d[STOPY] ) && mask_d[rank] != UNDEFINED ) {
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
  
  /*
  //private:
    __constant__ int start_x;
    __constant__ int start_y;
    __constant__ int stop_x;
    __constant__ int stop_y;
    __constant__ int size_x;
    __constant__ int size_y;
    
    __constant__ int step = 0;
    
    __constant__ bool found = false;
    
    Tensor mask_tensor;
    Tensor dist_tensor;

    Tensor input_tensor;
    Tensor * output_tensor;
    
    __constant__ float wall_height;
    */
    
    
    __device__ int get_x(int rank) { return -1 + rank - (size_x * (  (int) ( rank / size_x ) )) ; }
    __device__ int get_y(int rank) { return 0 +  (int) rank / size_x ; }
    __device__ int get_rank(int x, int y) {return 1+ ( (y ) * size_x ) + x  ; }
    
    __device__ bool near_visited( int rank) { 
        auto mask = mask_tensor.template flat<int32>();
        
        if(found) return false;
        
        if (mask_d[rank] == WALL){
            return false;
        }
        //right
        if (get_y(rank) == get_y(rank + 1) && rank + 1 <= size_x * size_y ) {
            if (mask_d[rank + 1] != UNDEFINED && 
                mask_d[rank + 1] != WALL ) return true;
        }
        //left
        if (get_y(rank) == get_y(rank - 1) && rank - 1 >= 0 ) {
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
        if (get_rank(start_x, start_y) == rank) {
            //std::cout << "start!\n";
            return true; 
        }
        
        return false; 
    }
  
    __device__ void must_check(int test , int rank) {
    
        auto mask = mask_tensor.template flat<int32>();
        auto dist = dist_tensor.template flat<int32>();
        auto grid = input_tensor.template flat<int32>();

        auto prev = output_tensor->flat<int32>();
        
        
        
        float a = WALL_MULT * sqrt ( 1 + pow(grid_d[test] - grid_d[rank], 2) );
        int d =  dist_d[rank] + (int) a;
        
        if (  ! wall_found(test,rank) && mask_d[rank] != WALL && mask_d[test] != WALL) {
            
            
            if ((d < dist_d[test] || dist_d[test] == 0)   ){
                if (true || get_x(rank) != start_x || get_y(rank) != start_y) {
                    
                    //if(get_rank(start_x,start_y) == rank) std::cout << "start " << rank << " " << start_x << " " << start_y <<"\n";
                    prev_d[test] = rank ;
                    dist_d[test] = d;
                    
                }
                
            
            }
        }
        
    }
    
    __device__ bool wall_found(int test, int rank) {
    
        auto mask = mask_tensor.template flat<int32>();
        auto grid = input_tensor.template flat<int32>();
        
        float a = WALL_MULT * sqrt ( 1 + pow(grid_d[test] - grid_d[rank], 2) );
        if ((a > wall_height * WALL_MULT && grid_d[rank] <= grid_d[test] )|| mask_d[test] == WALL  ) {
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

    void DijkstraGridGpuLauncher(int * grid_d, int * prev_d, int * mask_d, int * dist_d, 
                                int * vars_d) {
                                
        DijkstraGridGpu  <<< 1, size_x * size_y >>>
                                (grid_d, prev_d, mask_d, dist_d, vars_d);
    }

#endif

