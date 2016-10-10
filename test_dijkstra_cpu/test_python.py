import tensorflow as tf
dijkstra_grid_module = tf.load_op_library('tensorflow/core/user_ops/dijkstra_grid.so')

with tf.Session(''):
  print(dijkstra_grid_module.dijkstra_grid([[1, 2], [3, 4]]).eval())

