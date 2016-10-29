## test with character maze of 8x8
#./test_op.py -height 8 -width 8  -stopx 7 -stopy 6 -startx 4 -starty 0 -w-height 20.5 -randomize-floor

## takes less than a second on cpu
#./test_op.py -mapname map.png -width 80 -height 80 -startx 65 -starty 74 -stopx 35 -stopy 9 

## takes less than a second on cpu
#./test_op.py -mapname map.png -width 80 -height 80

## takes at least 2min 21sec on cpu
#./test_op.py -mapname img/maze.png -width 480 -height 480



./test_op.py -mapname img/map.png -width 60 -height 60 -gpu -w-height 50

#./test_op.py -height 8 -width 8  -stopx 6 -stopy 7 -startx 4 -starty 0 -gpu -w-height 50 

#./test_op.py -height 40 -width 40  -stopx 6 -stopy 39 -startx 4 -starty 0 -gpu -w-height 50 
