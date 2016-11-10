#!/usr/bin/python

##import setup_readfile as setup 
import dijkstra
##import setup_png as png

if __name__ == '__main__': 

    d = dijkstra.Dijkstra()
    
    d.set_gpu(True)
    width = 8 
    height = 8 
    startx = 4 
    starty = 0 
    endx = 7 
    endy = 6 
    
    WALL =  100
    maze = [0] * (width * height) 
    dist = [0] * (width * height)
    prev = [0] * (width * height)
    visited = [0] * (width * height)
    
    
    onethird = int(height / 3)
    twothirds = int(height * 2 / 3 )
    
    for i in range (0, 6) : #(0,7)
        maze[ (onethird * width) + i] = WALL

    for i in range (4, width) :
        maze[ (twothirds * width) + i] = WALL
    
    d.set_maze(maze)
    d.set_startx(startx)
    d.set_starty(starty)
    d.set_stopx(endx)
    d.set_stopy(endy)
    d.set_width(width)
    d.set_height(height)
    d.set_wall_height(10.0)
    d.set_maze_printout_wall_height(10 + 2)

    d.eval()
    
