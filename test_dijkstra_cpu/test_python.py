#!/usr/bin/python

import setup_readfile as setup 
import dijkstra


if __name__ == '__main__': 
    s = setup.SU()
    d = dijkstra.Dijkstra()
    
    maze = s.get_maze()
    #print maze
    for i in range(len(maze)):
        if maze[i] == 1 : maze[i] = 5
    
    d.set_maze(maze)
    d.set_startx(s.get_startx())
    d.set_starty(s.get_starty())
    d.set_stopx(s.get_endx())
    d.set_stopy(s.get_endy())
    d.set_width(s.get_width())
    d.set_height(s.get_height())
    
    d.eval()
    
    d.print_output(s.get_maze() )
    print '----'
    d.set_special_printout(True)
    d.print_output(s.get_maze() ,d.get_output())

        
    print '----'
    d.set_special_printout(False)
    d.print_output(s.get_maze() ,d.get_output())


