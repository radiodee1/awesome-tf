#!/usr/bin/python

import sys, math
import fileinput
import random
import math

RANDOMIZED_FLOORS = 16

class SU(object):

    def __init__(self) :

        self.width = -1 #28#30
        self.height = -1 #28#30

        
        self.dim = []
        self.wall = []
        self.wallout = []
        self.csv = False
        self.gui = False
        self.output = False
        #self.dim_input = 0
        self.wall_height = 10
        self.randomize = False
        self.mapname = "map.png"

        self.startx = -1 #3
        self.starty = -1 #0
        self.endx = -1 #self.width - 3
        self.endy = -1 #self.height - 1

        self.all_coordinates = False
        self.gpu = False

        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-size':
                    self.dim_input = int(sys.argv[j+1])
                    if self.dim_input > 480 : self.dim_input = 480

        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-output':
                    self.output = True


        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-nogui':
                    self.gui = False
        
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-help':
                    print """
                    these are the flags:
                        -size <num>
                        -output
                        -nogui
                        -help
                        -startx <num>
                        -starty <num>
                        -stopx <num>
                        -stopy <num>
                        -width <num>
                        -height <num>
                        -w-height <num>
                        -randomize-floor
                        -mapname <filename>
                        -gpu
                    """
                    exit();
                    
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-startx':
                    self.startx = int(sys.argv[j+1]) #-1
                    #if self.dim_input > 480 : self.dim_input = 480

        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-starty':
                    self.starty = int(sys.argv[j+1]) # -1
                    #if self.dim_input > 480 : self.dim_input = 480

        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-stopx':
                    self.endx = int(sys.argv[j+1]) #-1
                    #if self.dim_input > 480 : self.dim_input = 480

        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-stopy':
                    self.endy = int(sys.argv[j+1]) #-1
                    #if self.dim_input > 480 : self.dim_input = 480
                    
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-height':
                    self.height = int(sys.argv[j+1]) 
                    #print self.height , "height"

        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-width':
                    self.width = int(sys.argv[j+1]) 
                    
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-w-height':
                    self.wall_height = float(sys.argv[j+1]) 
                    
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-randomize-floor':
                    self.randomize = True
                    
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-mapname':
                    self.mapname = str(sys.argv[j+1]) 
                    self.gui = True
                    
        if len(sys.argv) > 1:
            for j in range(0, len(sys.argv)):
                if sys.argv[j] == '-gpu':
                    self.gpu = True
        
        if self.gui == True:
            dim = 100
            if self.width == -1 : self.width = dim
            if self.height == -1 : self.height = dim
            if self.startx != -1 and self.starty != -1 and \
                    self.endx != -1 and self.endy != -1 :
                self.all_coordinates = True
        
        if self.startx == -1 : self.startx = 0
        if self.starty == -1 : self.starty = 0
        if self.endx == -1 : self.endx = 0
        if self.endy == -1 : self.endy = 0
        
        i = 0
        k = 0
        if len(sys.argv) > 1 : 
            #csv = True
    
            for j in range(0, len(sys.argv)):
                if sys.argv[j].endswith('.txt'):
                    self.csv = True
                    k = j
                    for line in fileinput.input(sys.argv[k]):
                        if not '#' in line[0] :
                            i += 1
                            if i == 1:
                                #print 'dim:', line
                                dim = line.split(',')
                                if self.width == -1 : self.width = int(dim[0])
                                if self.height == -1 : self.height = int(dim[1])
                            if i == 2:
                                #print 'wall-csv:', line
                                self.wall = line.split(',')
        

        self.starttime = 0
        self.endtime = 0

        
        #enum for maze
        self.OPEN = 0
        self.WALL =  100
        self.START = 0 # 2
        self.END = 0 # 3
        #self.PATH = 5

        #enum for visited
        self.VISITED = 1
        self.FREE = 0

        #enum for distance
        self.UNDEFINED =  -1 #1600#-1


        self.maze = [0] * (self.width * self.height) 
        self.dist = [self.UNDEFINED] * (self.width * self.height)
        self.prev = [self.UNDEFINED] * (self.width * self.height)
        self.visited = [0] * (self.width * self.height)


        #self.dist[(self.starty * self.width) + self.startx] = 0
        
        if self.randomize :
            for i in range(self.height * self.width)  :
                self.maze[i] = int(math.floor(random.random() * RANDOMIZED_FLOORS ))

        # wall from file input
        if self.csv == True:
            for i in self.wall :
                #print int(i)
                if i != '':
                    if int(i) < self.width * self.height:
                        #print 'add wall'
                        self.maze[int(i)] = self.WALL

        # non-random walls
        if self.csv == False and self.gui == False:
            onethird = int(self.height / 3)
            twothirds = int(self.height * 2 / 3 )
            for i in range (0, 6) : #(0,7)
                self.maze[ (onethird * self.width) + i] = self.WALL
    
            for i in range (4, self.width) :
                self.maze[ (twothirds * self.width) + i] = self.WALL
    
    
    def get_dim(self): return self.dim
    def get_wall(self): return self.wall
    def get_wallout(self): return self.wallout
    def get_width(self): return self.width
    def get_height(self): return self.height
    def get_dim(self): return self.dim
    
    # maze dist prev visited
    
    def get_maze(self): return self.maze
    def get_dist(self): return self.dist
    def get_prev(self): return self.prev
    def get_visited(self): return self.visited
    
    # startx starty endx endy
    
    def get_startx(self): return self.startx
    def get_starty(self): return self.starty
    def get_endx(self): return self.endx
    def get_endy(self): return self.endy
    def get_wall_height(self): return self.wall_height
    def get_randomized_floors(self): return RANDOMIZED_FLOORS
    
    def get_mapname(self): return self.mapname
    def get_is_gui(self): return self.gui
        
