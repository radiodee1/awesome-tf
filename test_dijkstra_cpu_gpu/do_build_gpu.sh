cd tensorflow/core/user_ops

TF_INC=$(python -c 'import tensorflow as tf; print(tf.sysconfig.get_include())')

#g++ -std=c++11 -shared dijkstra_grid.cc -o dijkstra_grid.so -fPIC -I $TF_INC -O2 -D_GLIBCXX_USE_CXX11_ABI=0

nvcc -std=c++11 -c -o dijkstra_grid_gpu.cu.o dijkstra_grid_gpu.cu.cc \
-I $TF_INC -D GOOGLE_CUDA=1 -x cu -Xcompiler -fPIC --expt-relaxed-constexpr

g++ -std=c++11 -shared -o dijkstra_grid_gpu.so dijkstra_grid_gpu.cc \
dijkstra_grid_gpu.cu.o -I $TF_INC -fPIC -lcudart -D_GLIBCXX_USE_CXX11_ABI=0 -L /usr/lib/x86_64-linux-gnu/
