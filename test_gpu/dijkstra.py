import sys
import tensorflow as tf

START = -2
STOP = -3
WALL = 0
FREE = 0
APATH = -4
UNDEFINED = -1

class Dijkstra(object):

    def __init__(self) :
        print "simple dijkstra for tensorflow"
        self.output = ''
        self.dijkstra_grid_module = None# tf.load_op_library('')
        
        self.width = 8#30
        self.height = 8#30
        self.wall_height = 2
        
        self.dim = []
        self.wall = []
        self.found = []
        
        self.special_printout = False
        self.maze_printout_wall_height = 20

        self.startx = 3
        self.starty = 0# height - 1
        self.stopx = self.width - 3
        self.stopy =  self.height - 1
        
        # maze dist prev visited
        self.maze = []
        self.dist = []
        self.prev = []
        self.visited = []
        
        self.gpu = False
        
    def eval(self):
        #print  self.maze
        if self.gpu : 
            self.dijkstra_grid_module = tf.load_op_library('d_grid_gpu.so')
            with tf.Session(''):
                self.output = self.dijkstra_grid_module.d_grid_gpu(
                        self.maze,
                        start_x = self.startx,
                        start_y = self.starty,
                        stop_x = self.stopx,
                        stop_y = self.stopy,
                        size_x = self.width,
                        size_y = self.height,
                        wall_height = self.wall_height
                    ).eval()
                    
        else : 
            print " do simple cpu version here..."
        
    def get_output(self):
        return self.output
        
    def print_output(self, mz, path = []):
        output = mz
        
        for y in range (0 , self.height):
            for x in range (0, self.width):
                #self.maze[ (y * self.width) + x] = self.FREE
                if self.startx == x and self.starty == y :
                    self.maze[ (y * self.width) + x] = START #self.START
                elif self.stopx == x and self.stopy == y :
                    self.maze[ (y * self.width) + x] = STOP #self.END
                #else: self.maze [ (y * self.width) + x] = 0;
        
        if len(path) > 0 : 
            #self.special_printout = True
            self.follow_path(path)

        for y in range(self.height):
            for x in range(self.width):
                if ( y * self.width) + x < len(output) :
                    symbol = "0"
                    if self.maze[(y * self.width) + x] == START or \
                        self.maze[(y * self.width) + x] == STOP: symbol = "X"
                    elif self.maze[(y * self.width) + x] == FREE : symbol = "0"
                    elif self.maze[(y * self.width) + x] == APATH : symbol = "*"
                    elif self.maze[(y * self.width) + x] - self.maze[0]  >= \
                        self.maze_printout_wall_height : symbol = "+"
                    
                    if self.special_printout : 
                        symbol = (str(self.maze[(y * self.width) + x])) + ","
                        if len(symbol) == 3 : symbol = " " + symbol
                        if len(symbol) == 2 : symbol = "  " + symbol
                        if len(symbol) == 1 : symbol = "   " + symbol
                    
                    sys.stdout.write (symbol) #(str(output[(y * self.width) + x]))
            
            print
        sys.stdout.flush()
        
        #print 3 == self.get_rank(self.get_x(3), self.get_y(3)), self.get_x(3), self.get_y(3), "calibrate!!"
        
    def follow_path(self, prev):
        ## call this after end is found!! ##
        dim = self.width * self.height 
        i = 0 
        foundlist = []
    
        found = prev[((self.stopy ) * self.width) + (   self.stopx ) ]
        #print found, "stop", len(prev), prev
        
        #for i in self.output: 
        #	if int(i) is not int(0) : print i
        
        endloop = False
        while (found != -1  ) and  i < dim and not endloop :
            
            if found in foundlist: endloop = True
            
            if (self.get_x(found) == self.startx and self.get_y(found) == self.starty) : 
                endloop = True 
                #print len(foundlist)
                
            if found != UNDEFINED  :
                if not endloop: foundlist.append(int(found))
            else :
                print int( found / width), found - (int(found / width) * width), 'y,x'
            
            if not endloop: 
                #if self.special_printout : print found, prev[found]
                if (found != -1) : found = prev[found]
                i += 1
            
        self.found = foundlist
        #print foundlist, 'found', len(foundlist)
        print "----"
        
        i = 0
        while (i < dim) :
            if ( int(i) in foundlist) :#and self.maze[i] != START:
                symbol = int(APATH)
                if  self.special_printout : symbol = i
                self.maze[i] = int(symbol) #PATH
                #print i, APATH, symbol
            i += 1    
        
        
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
    
    def set_special_printout(self, s) : self.special_printout = s 
    def set_wall_height(self, h) : self.wall_height = h
    def set_maze_printout_wall_height(self, h) : self.maze_printout_wall_height = h
    def set_gpu(self, g) : self.gpu = g

    def get_x(self, rank) : return rank - (self.width * int(rank / self.width))
    def get_y(self, rank) : return int(rank / self.width)
    def get_rank(self, x, y ) : return ( y * self.width ) + x

