#include "tensorflow/core/framework/op.h"

#define WALL 1
#define FREE 0
#define VISITED 2


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
    const Tensor& input_tensor = context->input(0);
    //auto grid = input_tensor.flat<int32>();
    
    auto grid = input_tensor.template flat<int32>();
    // Create an output tensor
    
    //std::vector<int>* output_vector = NULL;
    
    //auto output_vector = outputs[0].vec<int32>();
    
    Tensor * output_tensor = NULL;
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({size_x * size_y}), &output_tensor));
    
    auto output = output_tensor->flat<int32>();
    
    //Tensor  mask_tensor;
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &mask_tensor));

    auto mask = mask_tensor.template flat<int32>();
    /*
    OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor.shape(),
                                                     &output_tensor));
    auto output = output_tensor->flat<int32>();

    // Set all but the first element of the output tensor to 0.
    */
    const int N = grid.size();
    for (int rank = 1; rank < N; rank++) {
        output.data()[rank] = mask.data()[rank];
        if (rank < size_x * size_y) {
            if (mask.data()[rank] != VISITED && mask.data()[rank] != WALL) {
                //right
                if (get_y(rank) == get_y(rank + 1) && 
                    rank + 1 < size_x * size_y && near_visited(rank)) {
                
                }
                
                //left
                if (get_y(rank) == get_y(rank - 1) && 
                    rank - 1 >= 0 && near_visited(rank)) {
                
                }
                //down
                if (rank + size_x < size_x * size_y && near_visited(rank)) {
                }
                //up
                if (rank - size_x >=0 && near_visited(rank)) {
                }
                //start
                if (get_x(rank) == start_x && get_y(rank) == start_y) {
                }
                if (near_visited(rank)) {
                    mask.data()[rank] = VISITED;
                }
            }
        
        }

    }

    // Preserve the first input value if possible.
    //if (N > 0) output(0) = input(0);
    
  }
  
  
  private:
    int start_x;
    int start_y;
    int stop_x;
    int stop_y;
    int size_x;
    int size_y;
    
    Tensor mask_tensor;
    
    int get_x(int rank) { return rank - (size_x * ( rank / size_x ) ) ; }
    int get_y(int rank) { return rank / size_x ; }
    
    bool near_visited(int rank) { 
        auto mask = mask_tensor.template flat<int32>();
        //right
        if (get_y(rank) == get_y(rank + 1) && rank + 1 < size_x * size_y ) {
        }
        //left
        if (get_y(rank) == get_y(rank - 1) && rank - 1 >= 0 ) {
        }
        //down
        //up
        
        
        return true; 
    }
  
};

REGISTER_KERNEL_BUILDER(Name("DijkstraGrid").Device(DEVICE_CPU), DijkstraGridOp);

