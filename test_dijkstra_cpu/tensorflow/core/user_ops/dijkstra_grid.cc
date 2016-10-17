#include <cmath>

#include "tensorflow/core/framework/op.h"

#define WALL -1
#define FREE 0
#define VISITED 1
#define UNDEFINED 0

#define WALL_MULT 100

REGISTER_OP("DijkstraGrid")
    .Input("grid: int32")
    .Attr("start_x: int = 0")
    .Attr("start_y: int = 0")
    .Attr("stop_x: int = 28")
    .Attr("stop_y: int = 28")
    .Attr("size_x: int = 28")
    .Attr("size_y: int = 28")
    .Attr("wall_height: float = 2.5")
    .Output("prev: int32");

#include "tensorflow/core/framework/op_kernel.h"


using namespace tensorflow;

class DijkstraGridOp : public OpKernel {
  public:
  explicit DijkstraGridOp(OpKernelConstruction* context) : OpKernel(context) {
  
    OP_REQUIRES_OK(context,
                   context->GetAttr("start_x", &start_x));
    OP_REQUIRES_OK(context,
                   context->GetAttr("start_y", &start_y));
    OP_REQUIRES_OK(context,
                   context->GetAttr("stop_x", &stop_x));
    OP_REQUIRES_OK(context,
                   context->GetAttr("stop_y", &stop_y));
    OP_REQUIRES_OK(context,
                   context->GetAttr("size_x", &size_x));
    OP_REQUIRES_OK(context,
                   context->GetAttr("size_y", &size_y));
    OP_REQUIRES_OK(context,
                   context->GetAttr("wall_height", &wall_height));
  
  }

  void Compute(OpKernelContext* context) override {
    // Grab the input tensor
    
    input_tensor = context->input(0);
    auto grid = input_tensor.template flat<int32>();
    
    
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({size_x * size_y}), &output_tensor));
    
    auto prev = output_tensor->flat<int32>();
    
    
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &mask_tensor));
    auto mask = mask_tensor.template flat<int32>();
    
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &dist_tensor));
    auto dist = dist_tensor.template flat<int32>();
    
    const int N = grid.size();
    step = 0;
    
    for (int rank = 0; rank < N; rank++) prev.data()[rank] = UNDEFINED;
    mask.data() [get_rank(start_x, start_y)] = VISITED;
    prev.data()[get_rank(start_x, start_y)] = -1;
    dist.data()[get_rank(start_x, start_y)] = 1;

    
    while( !found && step < size_x * size_y +1) {
        step ++;
        //std::cout << step << " = while loop i \n";
        //print();
        for (int rank = 0; rank < N; rank++) {
            
            if (! found) { 
            
                
                if (  mask.data()[rank] != WALL  ) { 
                    //right
                    if (get_y(rank) == get_y(rank + 1) && 
                            rank + 1 < size_x * size_y && near_visited( rank) ) { 
                        //std::cout << "right\n";
                        if (! wall_found(rank+1,rank)) {
                            
                            
                            must_check(rank + 1, rank);
                            //break;
                        }
                        else {
                            mask.data()[rank+1 ] = WALL;
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
                            mask.data()[rank -1] = WALL;
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
                            mask.data()[rank + size_x] = WALL;
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
                            mask.data()[rank  - size_x] = WALL;
                        }
                    }
                    
                    if (near_visited(rank) ){
                        
                        if (true || mask.data()[rank] == UNDEFINED) mask.data()[rank] = step;
                    }
                    if ( rank == get_rank(stop_x,stop_y) && mask.data()[rank] != UNDEFINED ) {
                        found = true;
                        //std::cout << "stop here !!\n" ;
                    }

                    
                }
            
            }

        } // for
    } // while

    
    //std::cout << "loop = " << step << ", wall = " << wall_height << "\n";
    
    //print();
    
    
  }
  
  
  private:
    int start_x;
    int start_y;
    int stop_x;
    int stop_y;
    int size_x;
    int size_y;
    
    int step = 0;
    
    bool found = false;
    
    Tensor mask_tensor;
    Tensor dist_tensor;
    //Tensor prev_tensor;
    Tensor input_tensor;
    Tensor * output_tensor;
    
    float wall_height;
    
    int get_x(int rank) { return -1 + rank - (size_x * (  (int) ( rank / size_x ) )) ; }
    int get_y(int rank) { return 0 +  (int) rank / size_x ; }
    int get_rank(int x, int y) {return 1+ ( (y ) * size_x ) + x  ; }
    
    bool near_visited( int rank) { 
        auto mask = mask_tensor.template flat<int32>();
        
        if(found) return false;
        
        if (mask.data()[rank] == WALL){
            return false;
        }
        //right
        if (get_y(rank) == get_y(rank + 1) && rank + 1 <= size_x * size_y ) {
            if (mask.data()[rank + 1] != UNDEFINED && 
                mask.data()[rank + 1] != WALL ) return true;
        }
        //left
        if (get_y(rank) == get_y(rank - 1) && rank - 1 >= 0 ) {
            if (mask.data()[rank - 1] != UNDEFINED && 
                mask.data()[rank - 1] != WALL) return true;
        }
        //down
        if (rank + size_x < size_x * size_y) {
            if (mask.data()[rank + size_x] != UNDEFINED && 
                mask.data()[rank + size_x] != WALL ) return true;
        }
        //up
        if (rank - size_x >=0) {
            if (mask.data()[rank - size_x] != UNDEFINED && 
                mask.data()[rank - size_x ] != WALL) return true;
        }
        if (get_rank(start_x, start_y) == rank) {
            //std::cout << "start!\n";
            return true; 
        }
        
        return false; 
    }
  
    void must_check(int test , int rank) {
    
        auto mask = mask_tensor.template flat<int32>();
        auto dist = dist_tensor.template flat<int32>();
        auto grid = input_tensor.template flat<int32>();

        auto prev = output_tensor->flat<int32>();
        
        
        
        float a = WALL_MULT * sqrt ( 1 + pow(grid.data()[test] - grid.data()[rank], 2) );
        int d =  dist.data()[rank] + (int) a;
        
        if (  ! wall_found(test,rank) && mask.data()[rank] != WALL && mask.data()[test] != WALL) {
            
            
            if ((d < dist.data()[test] || dist.data()[test] == 0)   ){
                if (true || get_x(rank) != start_x || get_y(rank) != start_y) {
                    
                    //if(get_rank(start_x,start_y) == rank) std::cout << "start " << rank << " " << start_x << " " << start_y <<"\n";
                    prev.data()[test] = rank ;
                    dist.data()[test] = d;
                    
                }
                
            
            }
        }
        
    }
    
    bool wall_found(int test, int rank) {
        auto mask = mask_tensor.template flat<int32>();
        auto grid = input_tensor.template flat<int32>();
        float a = WALL_MULT * sqrt ( 1 + pow(grid.data()[test] - grid.data()[rank], 2) );
        if ((a > wall_height * WALL_MULT && grid.data()[rank] <= grid.data()[test] )|| mask.data()[test] == WALL  ) {
            //std::cout << test << " " << rank << " " << a << " -- wall!!\n";
            return true; 
        }
        return false;
    }
    
    void print() {
        auto mask = input_tensor.template flat<int32>();

        for (int rank = 0; rank < mask.size(); rank++) {
            std::cout << mask.data()[rank] << ",";
            if (rank % size_x == size_x - 1) std::cout << "\n";
        }
        std::cout << "\n";

        
    }
};

REGISTER_KERNEL_BUILDER(Name("DijkstraGrid").Device(DEVICE_CPU), DijkstraGridOp);

