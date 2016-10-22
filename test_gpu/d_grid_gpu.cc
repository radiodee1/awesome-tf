//#include <cmath>

#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"

REGISTER_OP("DGridGpu")
    .Input("grid: int32")
    .Attr("start_x: int = 0")
    .Attr("start_y: int = 0")
    .Attr("stop_x: int = 28")
    .Attr("stop_y: int = 28")
    .Attr("size_x: int = 28")
    .Attr("size_y: int = 28")
    .Attr("wall_height: float = 2.5")
    .Output("prev: int32");

using namespace tensorflow;
    
    void run();
    
class DGridGpuOp : public OpKernel {
  public:
  explicit DGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    
  }

  void Compute(OpKernelContext* context) override {
    
    Tensor grid;
    Tensor * prev;
    
    grid = context->input(0);
    auto grid_host = grid.template flat<int32>();
    
    
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({64}), &prev));
    
    auto prev_host = prev->flat<int32>();
    
    run(); // do something to grid_host and move it to grid_prev
    
    //exit
  }
  
};

//REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_GPU).HostMemory("grid").HostMemory("prev"), DGridGpuOp);
REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_CPU), DGridGpuOp);
