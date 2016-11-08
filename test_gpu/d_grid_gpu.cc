

//#if GOOGLE_CUDA
//#define EIGEN_USE_GPU
//#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"



using namespace tensorflow;
    
REGISTER_OP("GridGpu").Input("grid: int32").Output("prev: int32");    
    
    void run(const int * in, int * out);
    
class DGridGpuOp : public OpKernel {
  public:
  explicit DGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    
  }

  void Compute(OpKernelContext* context) override {
    
    
    Tensor* prev_tensor = NULL;
    
    Tensor grid_tensor = context->input(0);
    
    auto grid = grid_tensor.flat<int32>();    
    
    
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, 
                                 TensorShape({64}), &prev_tensor));
    
    auto prev = prev_tensor->template flat<int32>();
    
    
    run(grid.data(), prev.data());
    
    
  }
  
};

REGISTER_KERNEL_BUILDER(Name("GridGpu").Device(DEVICE_GPU), DGridGpuOp);

//REGISTER_KERNEL_BUILDER(Name("DGridGpu").Device(DEVICE_CPU), DGridGpuOp);

//#endif
