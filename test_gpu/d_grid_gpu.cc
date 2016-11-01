

#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"

REGISTER_OP("DGridGpu")
    .Input("grid: int32")
    .Output("prev: int32");

using namespace tensorflow;
    
    void run( int * in);//, int * out);
    
class DGridGpuOp : public OpKernel {
  public:
  explicit DGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    
  }

  void Compute(OpKernelContext* context) override {
    
    
    Tensor* prev_tensor = NULL;
    
    const Tensor& grid_tensor = context->input(0);
    
    auto grid = grid_tensor.flat<int32>();    
    
    
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({64}), &prev_tensor));
    
    auto prev = prev_tensor->template flat<int32>();
    
    
    run(prev.data());//, prev.data()); // do something to grid_tensor and move it to prev_tensor
    
    
    //exit
  }
  
};

REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_GPU).HostMemory("grid").HostMemory("prev"), DGridGpuOp);
//REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_CPU), DGridGpuOp);
