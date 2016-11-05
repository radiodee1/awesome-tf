TF_INC=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')

nvcc -std=c++11 -c -o cuda_op_kernel.cu.o cuda_op_kernel.cu.cc \
-I $TF_INC -D GOOGLE_CUDA=1 -x cu -Xcompiler -fPIC --expt-relaxed-constexpr

g++ -std=c++11 -shared -o cuda_op_kernel.so cuda_op_kernel.cc \
cuda_op_kernel.cu.o -I $TF_INC -fPIC -lcudart -D_GLIBCXX_USE_CXX11_ABI=0 -L /usr/lib/x86_64-linux-gnu/
