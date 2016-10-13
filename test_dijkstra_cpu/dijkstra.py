import sys
import tensorflow as tf

class Dijkstra(object):

    def __init__(self) :
        print "simple dijkstra for tensorflow"
        self.output = ''
        self.dijkstra_grid_module = tf.load_op_library('tensorflow/core/user_ops/dijkstra_grid.so')
        
        self.width = 8#30
        self.height = 8#30

        
        self.dim = []
        self.wall = []
        self.wallout = []
        #self.csv = False
        #self.gui = False
        #self.output = False
        #self.dim_input = 0
        #self.single_kernel = False

        self.startx = 3
        self.starty = 0# height - 1
        self.stopx = self.width - 3
        self.stopy =  self.height - 1
        
        # maze dist prev visited
        self.maze = []
        self.dist = []
        self.prev = []
        self.visited = []
        
    def eval(self):
        print (self.width, self.height)
        
        with tf.Session(''):
            self.output = self.dijkstra_grid_module.dijkstra_grid(
                    self.maze,
                    start_x = self.startx,
                    start_y = self.starty,
                    stop_x = self.stopx,
                    stop_y = self.stopy,
                    size_x = self.width,
                    size_y = self.height
                ).eval()
        
        
    def get_output(self):
        return self.output
        
    def print_output(self, mz):
        output = mz
        
        for y in range (0 , self.height):
            for x in range (0, self.width):
                #self.maze[ (y * self.width) + x] = self.FREE
                if self.startx == x and self.starty == y :
                    self.maze[ (y * self.width) + x] = 2 #self.START
                if self.stopx == x and self.stopy == y :
                    self.maze[ (y * self.width) + x] = 3 #self.END
            
        
        for y in range(self.height):
            for x in range(self.width):
                if ( y * self.width) + x < len(output) :
                    sys.stdout.write (str(output[(y * self.width) + x]))
            print
        sys.stdout.flush()
        
    # start stop
    def set_startx(self , x): self.startx = x
    def set_starty(self , y): self.starty = y
    def set_stopx(self , x): self.stopx = x
    def set_stopy(self , y): self.stopy = y
    
    def set_maze(self , m): self.maze = m
    def set_dist(self , d): self.dist = d
    def set_prev(self , p): self.prev = p
    def set_visited(self , v): self.visited = v
    
    # width height
    
    def set_width(self, w): self.width = w
    def set_height(self, h ): self.height = h

