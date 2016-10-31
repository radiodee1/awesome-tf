

#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"

REGISTER_OP("DGridGpu")
    .Input("grid: int32")
    .Output("prev: int32");

using namespace tensorflow;
    
    void run(const int * in, int * out);
    
class DGridGpuOp : public OpKernel {
  public:
  explicit DGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    
  }

  void Compute(OpKernelContext* context) override {
    
    
    Tensor* prev_h = NULL;
    
    const Tensor& grid_h = context->input(0);
    
    auto grid = grid_h.flat<int32>();    
    
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({64}), &prev_h));
    
    auto prev = prev_h->flat<int32>();
    
    run(grid.data(), prev.data()); // do something to grid_host and move it to prev_host
    
    //exit
  }
  
};

REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_GPU), DGridGpuOp);
//REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_CPU), DGridGpuOp);
