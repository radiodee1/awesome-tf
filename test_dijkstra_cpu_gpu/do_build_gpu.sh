export CC=gcc-5
export CXX=g++-5

cd tensorflow/core/user_ops

TF_INC=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')


nvcc  -std=c++11 -c -o dijkstra_grid_gpu.cu.o dijkstra_grid_gpu.cu.cc \
-I $TF_INC -D GOOGLE_CUDA=1 -x cu -Xcompiler -fPIC --expt-relaxed-constexpr  

# -arch=sm_30 # -arch=compute_11 #-arch=sm_52

g++-5 -std=c++11 -shared -o dijkstra_grid_gpu.so dijkstra_grid_gpu.cc \
dijkstra_grid_gpu.cu.o -I $TF_INC -fPIC -lcudart -D_GLIBCXX_USE_CXX11_ABI=0 -L /usr/lib/nvidia-375/

#g++ -std=c++11 -shared -o dijkstra_grid_gpu.so dijkstra_grid_gpu.cc \
#dijkstra_grid_gpu.cu.o -I $TF_INC -fPIC -lcudart -D_GLIBCXX_USE_CXX11_ABI=0 -L /usr/local/cuda-8.0/lib64/

#  /usr/lib/nvidia-cuda-toolkit
