#!/usr/bin/python

import setup_readfile as setup 
import dijkstra
import setup_png as png

if __name__ == '__main__': 
    s = setup.SU()
    d = dijkstra.Dijkstra()
    
    d.set_gpu(s.gpu)
    
    if not s.get_is_gui() :
        d.set_maze(s.get_maze())
        d.set_startx(s.get_startx())
        d.set_starty(s.get_starty())
        d.set_stopx(s.get_endx())
        d.set_stopy(s.get_endy())
        d.set_width(s.get_width())
        d.set_height(s.get_height())
        d.set_wall_height(s.get_wall_height())
        d.set_maze_printout_wall_height(s.get_randomized_floors() + 2)
    if s.get_is_gui():
        p = png.Interface(s)
        p.set_mapname(s.get_mapname())
        p.solve_png(d,s)
        

    
    if not s.get_is_gui():
        d.eval()
        print "maze with elevations"
        d.set_special_printout(True) 
        d.print_output(s.get_maze() )
        
        print '----'
        print "maze path"
        d.set_special_printout(False)
        d.print_output(s.get_maze() ,d.get_output())

