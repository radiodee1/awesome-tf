
from Tkinter import Tk
from tkFileDialog import askopenfilename

import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk


class Interface(Gtk.Window) :
    
    
    def __init__(self) :
        #self.mz = []
        self.mapname = ""
        self.quit = 0
        
        Gtk.Window.__init__(self, title="Maps")
        self.set_border_width(10)

        grid = Gtk.Grid()
        self.add(grid)
        
        self.button = Gtk.Button(label="Load Image")
        self.button.connect("clicked", self.on_image_button_clicked)
        grid.add(self.button)
        
        self.label = Gtk.Label("filename")
        grid.attach(self.label, 0, 1, 1, 1)
        
        self.entry = Gtk.Entry()
        self.entry.set_text("mapname")
        grid.attach(self.entry, 1, 1, 1, 1)
        
        self.confirm = Gtk.Button(label="Save Image")
        self.confirm.connect("clicked", self.on_confirm_button_clicked)
        grid.attach(self.confirm, 1, 0, 1, 1)
        
    def on_image_button_clicked(self, widget):
        print("Hello World")
        Tk().withdraw() 
        self.mapname = askopenfilename() 
        print(self.mapname)
        self.label.set_text(self.mapname)
        
    def on_confirm_button_clicked(self, widget):
        print(self.entry.get_text())
        #self.label.set_text(self.entry.get_text())
        
        
if __name__ == '__main__': 
    #a = Interface();
    
    win = Interface()
    win.connect("delete-event", Gtk.main_quit)
    win.show_all()
    Gtk.main()
