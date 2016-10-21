

TF_INC=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')


nvcc -std=c++11 -c -o d_grid_gpu.cu.o d_grid_gpu.cu.cc \
-I $TF_INC -D GOOGLE_CUDA=1 -x cu -Xcompiler -fPIC --expt-relaxed-constexpr

g++ -std=c++11 -shared -o d_grid_gpu.so d_grid_gpu.cc \
d_grid_gpu.cu.o -I $TF_INC -fPIC -lcudart -D_GLIBCXX_USE_CXX11_ABI=0 -L /usr/lib/x86_64-linux-gnu/
