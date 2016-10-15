#include "tensorflow/core/framework/op.h"

#define WALL 1
#define FREE 0
#define VISITED 2
#define UNDEFINED 0


REGISTER_OP("DijkstraGrid")
    .Input("grid: int32")
    .Attr("start_x: int = 0")
    .Attr("start_y: int = 0")
    .Attr("stop_x: int = 28")
    .Attr("stop_y: int = 28")
    .Attr("size_x: int = 28")
    .Attr("size_y: int = 28")
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
  
  }

  void Compute(OpKernelContext* context) override {
    // Grab the input tensor
    //const Tensor& input_tensor = context->input(0);
    //auto grid = input_tensor.flat<int32>();
    input_tensor = context->input(0);
    auto grid = input_tensor.template flat<int32>();
    // Create an output tensor
    
    
    Tensor * output_tensor = NULL;
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({size_x * size_y}), &output_tensor));
    
    auto output = output_tensor->flat<int32>();
    
    
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &mask_tensor));
    auto mask = mask_tensor.template flat<int32>();
    
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &dist_tensor));
    auto dist = dist_tensor.template flat<int32>();
    
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &prev_tensor));
    auto prev = prev_tensor.template flat<int32>();
    
    const int N = grid.size();
    int i = 0;
    
    //std::cout << "type " << typeid(mask).name() << "\n";
    for (int rank = 0; rank < N; rank++) prev.data()[rank] = UNDEFINED;
    
    while( !found && i < size_x * size_y) {
        i ++;
        for (int rank = 0; rank < N; rank++) {
            //output.data()[rank] = mask.data()[rank];
            if (rank < size_x * size_y) {
                if (mask.data()[rank] != VISITED && grid.data()[rank] != WALL) {
                    //right
                    if (get_y(rank) == get_y(rank + 1) && 
                            rank + 1 < size_x * size_y && near_visited( rank)) {
                        must_check(rank + 1, rank);
                    }
                    
                    //left
                    if (get_y(rank) == get_y(rank - 1) && 
                            rank - 1 >= 0 && near_visited(rank)) {
                        must_check(rank -1, rank);
                    }
                    //down
                    if (rank + size_x < size_x * size_y && near_visited(rank)) {
                        must_check(rank + size_x, rank);
                    }
                    //up
                    if (rank - size_x >=0 && near_visited(rank)) {
                        must_check(rank - size_x, rank);
                    }
                    //start
                    if (get_x(rank) == start_x && get_y(rank) == start_y) {
                        must_check(rank, rank);
                    }
                    if (near_visited(rank)) {
                        mask.data()[rank] = VISITED;
                    }
                    
                    if (rank == get_rank(stop_x,stop_y) && mask.data()[rank] == VISITED) {
                        found = true;
                    }
                }
            
            }

        } // for
    } // while

    //test();
    
    //output 
    for (int rank = 0; rank < N; rank++) {
        output.data()[rank] = prev.data()[rank];
    }
    
    
  }
  
  
  private:
    int start_x;
    int start_y;
    int stop_x;
    int stop_y;
    int size_x;
    int size_y;
    
    bool found = false;
    
    Tensor mask_tensor;
    Tensor dist_tensor;
    Tensor prev_tensor;
    Tensor input_tensor;
    
    
    int get_x(int rank) { return rank - (size_x * ( rank / size_x ) ) ; }
    int get_y(int rank) { return rank / size_x ; }
    int get_rank(int x, int y) {return ( y * size_x ) + x ; }
    
    bool near_visited( int rank) { 
        auto mask = mask_tensor.template flat<int32>();
        
        
        //right
        if (get_y(rank) == get_y(rank + 1) && rank + 1 < size_x * size_y ) {
            if (mask.data()[rank + 1] == VISITED ) return true;
        }
        //left
        if (get_y(rank) == get_y(rank - 1) && rank - 1 >= 0 ) {
            if (mask.data()[rank - 1] == VISITED ) return true;
        }
        //down
        if (rank + size_x < size_x * size_y) {
            if (mask.data()[rank + size_x] == VISITED ) return true;
        }
        //up
        if (rank - size_x >=0) {
            if (mask.data()[rank - size_x] == VISITED ) return true;
        }
        //special start
        if (rank == get_rank(start_x, start_y)) {
            return true;
        }
        
        return false; 
    }
  
    void must_check(int test , int rank) {
        auto mask = mask_tensor.template flat<int32>();
        auto dist = dist_tensor.template flat<int32>();
        auto grid = input_tensor.template flat<int32>();
        auto prev = prev_tensor.template flat<int32>();
        
        if (mask.data()[rank] != VISITED && grid.data()[rank] != WALL) {
            if (dist.data()[rank] + 1 <= dist.data()[test] || dist.data()[test] == UNDEFINED) {
                if (rank != get_rank(start_x, start_y)) {
                    prev.data()[test] = rank;
                    dist.data()[test] = dist.data()[rank] + 1;
                }
            
            }
        }
    }
    
    void test() {
        auto grid = input_tensor.template flat<int32>();
        auto prev = prev_tensor.template flat<int32>();
        for (int rank = 0; rank < grid.size(); rank++) prev.data()[rank] = grid.data()[rank];
        //prev.data()[8*7] = 7*7;
    }
};

REGISTER_KERNEL_BUILDER(Name("DijkstraGrid").Device(DEVICE_CPU), DijkstraGridOp);

