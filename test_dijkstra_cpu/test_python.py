import setup

import tensorflow as tf
dijkstra_grid_module = tf.load_op_library('tensorflow/core/user_ops/dijkstra_grid.so')

if __name__ == '__main__': 
    su = setup.SU()


    with tf.Session(''):
        print(dijkstra_grid_module.dijkstra_grid([[1, 2], [3, 4]]).eval())
        
    print su.get_height()

