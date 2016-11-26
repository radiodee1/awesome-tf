
import numpy
import time, math
#import fileinput
import pygame as pg
import pygame.gfxdraw as pgd
import pygame
import sys



class Interface(object) :

	def __init__(self, readfile):
		self.mz = readfile
		self.mapname = 'map.png'
		self.iconname = 'icon.png'
		self.map  = []
		self.quit = 0	
		self.maze = []
		self.maptitle = "dijkstra-prog"
		
		self.block_screen_1 = True ## select smaller map
		self.block_screen_2 = True ## select start and stop
		#self.block_screen_3 = True ## print results
		self.action_detect = False
		self.blit_instructions = False

		
	def solve_png(self , tfuser, readfile):
	
		
		x = 0
		y = 0
		white = (64, 64, 64)
		w = 480
		h = 480
		self.startx = -1
		self.starty = -1
		self.endx = -1
		self.endy = -1
		
		if readfile.all_coordinates == True :
				
			self.block_screen_1 = False ## select smaller map
			self.block_screen_2 = False ## select start and stop
			self.startx = readfile.startx
			self.starty = readfile.starty
			self.endx = readfile.endx
			self.endy = readfile.endy
		
		self.guiwidth = readfile.width
		self.guiheight = readfile.height
		
		surface = pg.image.load(self.mapname)
		icon = pg.image.load(self.iconname)

		pg.display.set_icon(icon)
		
		## initialize control components ##
		gray = (16,16,16)
		red =(255,0,0)
		green = (0,255,0)
		blue = (0,0,255)
		blocksize = (w /readfile.width) -2
		if blocksize <= 2 : blocksize = 4
		
		self.controldim =( blocksize +2) * 2  
		if self.controldim < 16 : self.controldim = 16
		#print blocksize , self.controldim
		
		self.boxborder = 5
		self.boxwidth = 3 * self.controldim # 48
		self.boxheight = self.controldim # 16
		self.box = pg.Surface((self.boxwidth + (self.boxborder * 2), 
			self.boxheight + (self.boxborder * 2)))
		self.box.fill(gray)
		boxred = pg.Surface((self.controldim,self.controldim))
		boxred.fill(red)
		boxgreen = pg.Surface((self.controldim,self.controldim))
		boxgreen.fill(green)
		boxblue = pg.Surface((self.controldim,self.controldim))
		boxblue.fill(blue)
		self.box.blit(boxgreen, (0 + self.boxborder,0 + self.boxborder))
		self.box.blit(boxred, (self.controldim + self.boxborder,0 + self.boxborder))
		self.box.blit(boxblue, (self.controldim * 2 + self.boxborder, 0 + self.boxborder))
		self.mousex = 0
		self.mousey = 0
		self.boundtop = h - (self.box.get_height() - self.boxborder)
		self.boundbottom = h - self.boxborder
		self.boundgreenleft = w - (self.box.get_width() -  self.boxborder)
		self.boundgreenright = w - (self.box.get_width() -  self.boxborder) + self.controldim #16
		self.boundredleft = w - (self.box.get_width() -  self.boxborder) + self.controldim #16
		self.boundredright = w - (self.box.get_width() -  self.boxborder) + self.controldim * 2 #32
		self.boundblueleft = w - (self.box.get_width() -  self.boxborder) + self.controldim * 2 #32
		self.boundblueright = w - (self.box.get_width() -  self.boxborder) + self.controldim * 3 #48
		
		
		self.startblock = pg.Surface((blocksize,blocksize))
		self.startblock.fill(green)
		self.endblock = pg.Surface((blocksize,blocksize))
		self.endblock.fill(red)
		self.pathblock = pg.Surface((blocksize,blocksize))
		self.pathblock.fill(blue)
		self.blockoffset = 0#blocksize / 2 
		
		## fixscale not used ##
		self.fixscale = ( float  ( w - (int ( w/ readfile.width  ) * readfile.width ))/w ) + 1
		
		## all walls ##
		self.wallbox = pg.Surface( (math.ceil((w/readfile.width)* self.fixscale), 
			math.ceil((h/readfile.height) * self.fixscale )))
		self.wallbox.fill((0,0,0))
		
		
		## display first screen ##
		screensurf = surface
		screen = pg.display.set_mode((w, h))
		pg.display.set_caption(self.maptitle, self.maptitle)
		screen.fill((white))
		
		## skip if all coordinates are specified at command line ##
		if self.block_screen_1 == True:
			self.quit = 0
			running = 1
			while running:

				for event in pg.event.get():
					if event.type == pg.QUIT:
						running = 0
						self.quit = 1	
					if event.type == pg.KEYUP:
						self.action_detect = True
						if event.key == pg.K_RETURN:
							running = 0
						if event.key == pg.K_UP:
							y -= 5
							if y < 0 : 
								y = 0
						if event.key == pg.K_DOWN:
							y += 5
							if y + readfile.height > screen.get_height() : 
								y = screen.get_height() - readfile.height 
							
						if event.key == pg.K_LEFT:
							x -= 5
							if x < 0 : 
								x = 0
						if event.key == pg.K_RIGHT:
							x += 5
							if x + readfile.width > screen.get_width() : 
								x = screen.get_width() - readfile.width 
											
				self.gui_instructions( screen, 1, event, w, h)
				
				screensurf = surface.copy()
				screen.fill(white)
				screen.blit(screensurf,(0,0))
				pgd.rectangle(screen, ((x,y),(readfile.width,readfile.height)), (255,0,0))
				if not self.blit_instructions : pg.display.flip()
			
		## display second screen ##
		screen.fill((white))
		self.smallsurf = pg.Surface((readfile.width, readfile.height))
		bwsurf = pg.Surface((readfile.width, readfile.height))
		self.smallsurf.blit(surface,(0,0),((x,y), (readfile.width, readfile.height)))
		
		
		pg.transform.threshold(bwsurf, self.smallsurf,
			(0,0,0,0),(20,20,20,0), (255,255,255,0), 1)	
		
		screensurf = pg.Surface((w,h))
		screensurf.fill((255,255,255))
		screen.fill((255,255,255))
		
		## convert to array representation ##
		self.sa = [0] * readfile.width * readfile.height
		self.maze = [0] * readfile.width * readfile.height
		pxarray = pygame.PixelArray(self.smallsurf)
		for yy in range (0, readfile.width):
			for xx in range (0, readfile.height):
				p =  pxarray[xx,yy]

				if p == 0 : p = self.mz.WALL
				else : p = 0
				self.sa[(yy * readfile.width) + xx] = p
				
				g = 0
				if p != 0 : g = self.mz.WALL
				else : g = 0
				self.maze[(yy * readfile.width) + xx] = g
				
				if p == self.mz.WALL:
					#self.mz.wallout.append((yy * readfile.width) + xx)
					
					## print walls to screen ! ##
					xxx = float(xx * ( self.fixscale)) 
					yyy = float(yy * ( self.fixscale)) 
					screensurf.blit(self.wallbox, 
						(float(xxx * float (w  / readfile.width))   ,
						float(yyy * float (h  / readfile.height))   ))
		
		self.gui_state = 0
		
		self.PLACE_START = 1
		self.PLACE_END = 2
		self.FIND_PATH = 3
		self.HOLD_START = 4
		self.HOLD_END = 5
		
		self.action_detect = False
		
		## skip if all coordinates are specified at command line ##
		if self.block_screen_2 == True :
			self.running = 1
			while self.running == 1 and self.quit == 0:
			
				for event in pg.event.get():
					if event.type == pg.QUIT:
						self.running = 0
						self.quit = 1	
			
				screen.fill((white))
				screen.blit(screensurf,(0,0))
				self.gui_controls(screen, event, w,h)
				self.gui_instructions(screen, 2, event, w, h )
				if not self.blit_instructions : pg.display.flip()
			
			print "-startx", self.startx
			print "-starty", self.starty
			print "-stopx", self.endx
			print "-stopy", self.endy
		
	
		
		self.mz.endx = self.endx
		self.mz.endy = self.endy
		
		self.mz.startx = self.startx
		self.mz.starty = self.starty
		
		if self.quit != 1:
			## run cpu calculation ## tfuser !!
			tfuser.set_maze(self.maze)
			tfuser.set_startx(self.startx)
			tfuser.set_starty(self.starty)
			tfuser.set_stopx(self.endx)
			tfuser.set_stopy(self.endy)
			tfuser.set_width(readfile.width)
			tfuser.set_height(readfile.height)
			tfuser.set_wall_height(readfile.get_wall_height())
			#d.set_maze_printout_wall_height(s.get_randomized_floors() + 2)
			
			starttime = time.clock()
			tfuser.eval()
			
			
			endtime = time.clock()
			print  endtime - starttime , 'time on processor'
			tfuser.follow_path(tfuser.output)
		
		## print screen with solution ##
		self.running = 1
		while self.running == 1 and self.quit == 0:
			
			for event in pg.event.get():
				if event.type == pg.QUIT:
					self.running = 0
					self.quit = 1	
			
				if event.type == pg.KEYUP:
					if event.key == pg.K_RETURN:
						self.running = 0
			
				if event.type == pg.MOUSEBUTTONDOWN:
			
					left , middle, right = pg.mouse.get_pressed() 
					if left == True:
						self.running = 0
			
			screen.fill((white))
			screen.blit(screensurf,(0,0))
			for i in tfuser.found :
				xx = i - ( readfile.get_width() * (int(i / readfile.get_width() )))
				yy = int(i / readfile.get_width())
			
				xxx = float(xx * ( self.fixscale)) 
				yyy = float(yy * ( self.fixscale)) 
				
				screen.blit(self.pathblock,
					(float(xxx * float(screen.get_width() / readfile.width)) + self.blockoffset,
					float(yyy * float(screen.get_width() / readfile.width)) + self.blockoffset))
				screen.blit(self.startblock,
					(self.startx * (screen.get_width() / readfile.width) * self.fixscale, 
					self.starty * (screen.get_width() / readfile.width) * self.fixscale))
				screen.blit(self.endblock,
					(self.endx * (screen.get_width() / readfile.width) * self.fixscale,
					self.endy * (screen.get_width() / readfile.width) * self.fixscale))
			pg.display.flip()

	def gui_controls(self, screen, event,w,h):
		# this helper function puts controls on the screen
		screen.blit(self.box ,( w- (self.box.get_width()), 
			h - (self.box.get_height())) )
		# detect mouse
		self.mousex , self.mousey = pg.mouse.get_pos()
		self.mousex = self.mousex - (self.wallbox.get_width() / 2)
		self.mousey = self.mousey - (self.wallbox.get_height() / 2)
		if event.type == pg.MOUSEBUTTONDOWN:
			#print 'here mouse'
			self.action_detect = True
			left , middle, right = pg.mouse.get_pressed() 
			if left == True:
				
				if self.mousey > self.boundtop \
						and self.mousey < self.boundbottom :
					if self.mousex > self.boundredleft and self.mousex < self.boundredright:
						self.gui_state = self.HOLD_END
						self.endx = -1
						self.endy = -1
					if self.mousex > self.boundgreenleft and self.mousex < self.boundgreenright:
						self.gui_state = self.HOLD_START
						self.startx = -1
						self.starty = -1
					if self.mousex > self.boundblueleft and self.mousex < self.boundblueright:
						self.running = 0
						self.gui_state = self.FIND_PATH

				elif self.gui_state == self.PLACE_START:
					if self.mousex < self.wallbox.get_width() * self.mz.width and \
							self.mousey < self.wallbox.get_height() * self.mz.height :
						self.startx = self.mousex / (screen.get_width() / self.smallsurf.get_width())
						self.starty = self.mousey / (screen.get_height()/ self.smallsurf.get_height())
						#self.gui_state = 0
						self.startx, self.starty = self.dot_not_on_wall(self.startx, self.starty)
					
				elif self.gui_state == self.PLACE_END:
					if self.mousex < self.wallbox.get_width() * self.mz.width and \
							self.mousey < self.wallbox.get_height() * self.mz.height :
						self.endx = self.mousex / (screen.get_width() / self.smallsurf.get_width())
						self.endy = self.mousey / (screen.get_height()/ self.smallsurf.get_height())
						#self.gui_state = 0
						self.endx, self.endy = self.dot_not_on_wall(self.endx, self.endy)
						
				elif self.gui_state == self.HOLD_START:
					self.gui_state = self.PLACE_START
				elif self.gui_state == self.HOLD_END:
					self.gui_state = self.PLACE_END
		
		
		
		if self.gui_state == self.HOLD_START:
			screen.blit(self.startblock,(self.mousex , self.mousey ))
					
		if self.gui_state == self.HOLD_END:
			screen.blit(self.endblock,(self.mousex, self.mousey))
					
		if (self.startx != -1 and self.starty != -1) :
		
			screen.blit(self.startblock,
				(self.startx * (screen.get_width() / self.smallsurf.get_width()) * self.fixscale, 
				self.starty * (screen.get_width() / self.smallsurf.get_width()) * self.fixscale))
		
		if (self.endx != -1 and self.endy != -1) :
			
			screen.blit(self.endblock,
				(self.endx * (screen.get_width() / self.smallsurf.get_width()) * self.fixscale,
				self.endy * (screen.get_width() / self.smallsurf.get_width()) * self.fixscale))
		
	def dot_not_on_wall(self, x, y) :
		xx = -1
		yy = -1
		x = int(x / self.fixscale)
		y = int(y / self.fixscale)
		if self.sa[(y * self.guiwidth) + x ] != self.mz.WALL : 
			xx = x
			yy = y  
			self.gui_state = 0
		return (xx,yy)

	def gui_instructions(self, screen, num, event, w, h):
		mousex , mousey = pg.mouse.get_pos()
		## mouse hover ##
		if (not self.action_detect) and mousex > 16 and mousey > 16 and \
				(mousey < self.boundtop and mousex < self.boundgreenleft ) :
			self.blit_instructions = True
			
			if   num == 0:
				surface = pg.image.load("img/instructions_0.png")
			elif num == 1:
				surface = pg.image.load("img/instructions_1.png")
			elif num == 2:
				surface = pg.image.load("img/instructions_2.png")
			screen.blit(surface,(0,0))
			pg.display.flip()
		else:  self.blit_instructions = False
		
		
			
	def show_maze(self, maze = [], width = 10, height = 10, symbols=True):
		
		if len(maze) != height * width:
			#
			print 'bad height * width'
		
		
		if True:	
			print
			for x in range (0,width + 2):
				if x < width + 1:
					print '#',
				else:
					print '#'

			for y in range (0 , height):
				print '#',
				for x in range (0, width):
					if symbols == True:
						if maze[ (y * width) + x] == self.mz.FREE :
							print ' ',
						elif maze[ (y * width) + x] == self.mz.START :
							print 'S',
						elif maze[ (y * width) + x] == self.mz.END :
							print 'X',
						elif maze[ (y * width) + x] == self.mz.WALL :
							print '#',
						elif maze[ (y * width) + x] == self.mz.PATH :
							print 'O',
						elif maze[ (y * width) + x] == self.mz.UNDEFINED :
							print 'U',
					else:
						print maze[ (y * width) + x] ,
				
				print '#',
				print

			for x in range (0,width + 2):
				print '#',
			print
			print
	
	def set_mapname(self, m) : self.mapname = m
	

