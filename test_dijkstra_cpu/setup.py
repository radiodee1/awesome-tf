#!/usr/bin/python

import sys, math
import fileinput

class SU(object):

	def __init__(self) :

		self.width = 8#30
		self.height = 8#30

		
		self.dim = []
		self.wall = []
		self.wallout = []
		self.csv = False
		self.gui = True
		self.output = False
		self.dim_input = 0
		self.single_kernel = False



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
		
		if self.gui == True:
			## 5 is the number of buffers (maze, dist, prev, visited, mutex) ##
			#dimension = int(math.sqrt(cl.device_info.MAX_WORK_GROUP_SIZE / 5))
			## if double kernel is used, 'dimension' is less important!! ##
			self.dimension = 60
			if self.dim_input != 0 : self.dimension = self.dim_input
			self.width = self.dimension
			self.height = self.dimension

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
								self.width = int(dim[0])
								self.height = int(dim[1])
							if i == 2:
								#print 'wall-csv:', line
								self.wall = line.split(',')
		

		self.starttime = 0
		self.endtime = 0

		self.startx = 3
		self.starty = 0# height - 1
		self.endx = self.width - 3
		self.endy =  self.height - 1

		#enum for maze
		self.OPEN = 1
		self.WALL = 2
		self.START = 3
		self.END = 4
		self.PATH = 5

		#enum for visited
		self.VISITED = 1
		self.FREE = 0

		#enum for distance
		self.UNDEFINED =  -1 #1600#-1


		self.maze = [0] * (self.width * self.height) 
		self.dist = [self.UNDEFINED] * (self.width * self.height)
		self.prev = [self.UNDEFINED] * (self.width * self.height)
		self.visited = [0] * (self.width * self.height)

		for y in range (0 , self.height):
			for x in range (0, self.width):
				self.maze[ (y * self.width) + x] = self.FREE
				if self.startx == x and self.starty == y :
					self.maze[ (y * self.width) + x] = self.START
				if self.endx == x and self.endy == y :
					self.maze[ (y * self.width) + x] = self.END
			

		self.dist[(self.starty * self.width) + self.startx] = 0

		self.maze[(self.starty * self.width) + self.startx] = self.START

		# wall from file input
		if self.csv == True:
			for i in self.wall :
				#print int(i)
				if i != '':
					if int(i) < self.width * self.height:
						#print 'add wall'
						self.maze[int(i)] = self.WALL

		# non-random walls
		if self.csv == False:
			onethird = int(self.height / 3)
			twothirds = int(self.height * 2 / 3 )
			for i in range (0, 6) : #(0,7)
				self.maze[ (onethird * self.width) + i] = self.WALL
	
			for i in range (4, self.width) :
				self.maze[ (twothirds * self.width) + i] = self.WALL
	
			# test for unreachable goal	
			#for i in range (0, width) :
			#	maze[ (twothirds * width) + i] = WALL

		## this doesn't take into account walls. ##
		def set_startvars(dist1, val):
			test = (self.starty * self.width) + self.startx
			if test >= 0 and test < self.width * self.height:
				if test + 1 < self.width * self.height and get_y(test) == get_y(test + 1):
					dist1[test + 1] = val
				if test - 1 >= 0 and get_y(test) == get_y(test - 1):
					dist1[test -1 ] = val
				if test + self.width < self.width * self.height:
					dist1 [test + self.width] = val
				if test - self.width >= 0 :
					dist1 [test - self.width] = val

		def get_y(ii):
			return int (ii / self.width)
