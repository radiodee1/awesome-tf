## test with character maze of 8x8
#./test_op.py -height 8 -width 8  -stopx 7 -stopy 6 -startx 4 -starty 0 -w-height 20.5 -randomize-floor

## takes less than a second on cpu
#./test_op.py -mapname map.png -width 80 -height 80 -startx 65 -starty 74 -stopx 35 -stopy 9 

## takes less than a second on cpu
#./test_op.py -mapname map.png -width 80 -height 80

## takes at least 2min 21sec on cpu
#./test_op.py -mapname img/maze.png -width 480 -height 480



#./test_op.py -mapname img/maze.png -width 480 -height 480 -gpu -w-height 50

## takes 9.6 secs... works most times -- doesn't freeze
./test_op.py -mapname img/map.png -width 50 -height 50 -gpu -w-height 50

#./test_op.py -height 18 -width 18  -stopx 6 -stopy 17 -startx 4 -starty 0  -w-height 50 -gpu

#./test_op.py -height 70 -width 70  -mapname img/smallmaze.png -gpu -w-height 50 

## takes 8.626 secs
#./test_op.py -mapname img/map.png -width 70 -height 70  -stopx 35 -stopy 6 -startx 47 -starty 57 -gpu -w-height 50 

## takes 0.2 secs
#./test_op.py -mapname img/map.png -width 70 -height 70  -stopx 35 -stopy 6 -startx 47 -starty 57  -w-height 50 
