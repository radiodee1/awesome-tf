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

class DGridGpuOp : public OpKernel {
  public:
  explicit DGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    
  }

  void Compute(OpKernelContext* context) override {
    
  }
    
  //private:
  
    
};

REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_GPU), DGridGpuOp);

