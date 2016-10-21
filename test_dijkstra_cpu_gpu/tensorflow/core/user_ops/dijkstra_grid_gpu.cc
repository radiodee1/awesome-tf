#include <cmath>

#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_kernel.h"

//#include "dijkstra_grid_gpu.h"

#define WALL -1
#define VISITED 1
#define UNDEFINED 0

#define WALL_MULT 100

REGISTER_OP("DijkstraGridGpu")
    .Input("grid: int32")
    .Attr("start_x: int = 0")
    .Attr("start_y: int = 0")
    .Attr("stop_x: int = 28")
    .Attr("stop_y: int = 28")
    .Attr("size_x: int = 28")
    .Attr("size_y: int = 28")
    .Attr("wall_height: float = 2.5")
    .Output("prev: int32");



    //void DijkstraGridGpuLauncher(int size_x, int size_y, int * grid_d, int * prev_d, int * mask_d, int * dist_d, int * vars) ;

using namespace tensorflow;

class DijkstraGridGpuOp : public OpKernel {
  public:
  explicit DijkstraGridGpuOp(OpKernelConstruction* context) : OpKernel(context) {
  
    /*
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
    */
  }

  void Compute(OpKernelContext* context) override {
    
    
    /*
    
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
    
    OP_REQUIRES_OK(context, context->allocate_temp(
                                DataTypeToEnum<int32>::value,
                                TensorShape({VARS_ARRAY_SIZE}), &vars_tensor));
    auto vars = vars_tensor.template flat<int32>();
    */
    
    //const int N = grid.size();
    //step = 0;
    
    /*
    for (int rank = 0; rank < prev.size(); rank++) prev.data()[rank] = UNDEFINED;
    mask.data() [get_rank(start_x, start_y)] = VISITED;
    prev.data()[get_rank(start_x, start_y)] = -1;
    dist.data()[get_rank(start_x, start_y)] = 1;

    vars.data()[STARTX] = start_x;
    vars.data()[STARTY] = start_y;
    vars.data()[STOPX] = stop_x;
    vars.data()[STOPY] = stop_y;
    vars.data()[SIZEX] = size_x;
    vars.data()[SIZEY] = size_y;
    vars.data()[WALLHEIGHT] = wall_height;
    vars.data()[FOUND] = 0;
    vars.data()[STEP] = 0;
    
    std::cout << VARS_ARRAY_SIZE<<" vars array size!! \n";

    int * grid_d, * prev_d, * mask_d, * dist_d, * vars_d;

    grid_d = grid.data();
    prev_d = prev.data();
    //mask_d = mask.data();
    //dist_d = dist.data();
    vars_d = vars.data();
    */

    //DijkstraGridGpuLauncher(size_x, size_y, grid_d, prev_d, mask_d, dist_d, vars_d) ;
                                

    
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
    
    /*
    Tensor mask_tensor;
    Tensor dist_tensor;
    Tensor vars_tensor;
    Tensor input_tensor;
    Tensor * output_tensor;
    */
    
    float wall_height;
    
    
    int get_x(int rank) { return -1 + rank - (size_x * (  (int) ( rank / size_x ) )) ; }
    int get_y(int rank) { return 0 +  (int) rank / size_x ; }
    int get_rank(int x, int y) {return 1+ ( (y ) * size_x ) + x  ; }
    
};

REGISTER_KERNEL_BUILDER(Name("DijkstraGridGpu").Device(DEVICE_GPU), DijkstraGridGpuOp);

