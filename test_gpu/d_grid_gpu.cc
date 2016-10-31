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
    
    void run(const int * in, int * out);
    
class DGridGpuOp : public OpKernel {
  public:
  explicit DGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    
  }

  void Compute(OpKernelContext* context) override {
    
    
    Tensor * prev;
    
    const Tensor& grid = context->input(0);
    
    auto grid_host = grid.flat<int32>();    
    
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({64}), &prev));
    
    auto prev_host = prev->template flat<int32>();
    
    run(grid_host.data(), prev_host.data()); // do something to grid_host and move it to prev_host
    
    //exit
  }
  
};

REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_GPU), DGridGpuOp);
//REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_CPU), DGridGpuOp);
