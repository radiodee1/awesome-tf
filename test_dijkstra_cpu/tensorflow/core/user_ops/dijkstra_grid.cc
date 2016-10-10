#include "tensorflow/core/framework/op.h"

REGISTER_OP("DijkstraGrid")
    .Input("grid: int32")
    .Attr("start_x: int = 0")
    .Attr("start_y: int = 0")
    .Attr("stop_x: int = 28")
    .Attr("stop_y: int = 28")
    .Attr("size_x: int = 28")
    .Attr("size_y: int = 28")
    .Output("path: int32");

#include "tensorflow/core/framework/op_kernel.h"
//#include "tensorflow/core/framework/tensor.h"

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
    auto grid = input_tensor.flat<int32>();

    // Create an output tensor
    
    //std::vector<int>* output_vector = NULL;
    
    //auto output_vector = outputs[0].vec<int32>();
    
    Tensor * output = NULL;
    OP_REQUIRES_OK(context, context->allocate_output(
                                 0, TensorShape({size_x * size_y}), &output));
    
    Tensor mask1;
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({size_x * size_y}), &mask1));

    /*
    OP_REQUIRES_OK(context, context->allocate_output(0, input_tensor.shape(),
                                                     &output_tensor));
    auto output = output_tensor->flat<int32>();

    // Set all but the first element of the output tensor to 0.
    const int N = input.size();
    for (int i = 1; i < N; i++) {
      output(i) = 0;
    }

    // Preserve the first input value if possible.
    if (N > 0) output(0) = input(0);
    */
  }
  private:
    int start_x;
    int start_y;
    int stop_x;
    int stop_y;
    int size_x;
    int size_y;
};

REGISTER_KERNEL_BUILDER(Name("DijkstraGrid").Device(DEVICE_CPU), DijkstraGridOp);

