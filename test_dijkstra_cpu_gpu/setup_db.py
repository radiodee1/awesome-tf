
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
        
        self.testarray =     [(0,"img/map.png" , "first map"),
                        (1,"img/smallmap.png","second map"),
                        (2,"img/maze.png", "maze"),
                        (5,"img/notthere.png","not there")]
        
        Gtk.Window.__init__(self, title="Maps")
        self.set_border_width(10)

        self.grid = Gtk.Grid()
        self.add(self.grid)
        
        self.button = Gtk.Button(label="Load Image")
        self.button.connect("clicked", self.on_image_button_clicked)
        self.grid.add(self.button)
        
        self.label = Gtk.Label("filename")
        self.grid.attach(self.label, 0, 1, 1, 1)
        
        self.entry = Gtk.Entry()
        self.entry.set_text("mapname")
        self.grid.attach(self.entry, 1, 1, 1, 1)
        
        self.confirm = Gtk.Button(label="Save Image")
        self.confirm.connect("clicked", self.on_confirm_button_clicked)
        self.grid.attach(self.confirm, 1, 0, 1, 1)
        
        
        self.liststore = self.make_list()
        self.treeview = Gtk.TreeView(self.liststore)
        
        renderer_path = Gtk.CellRendererText()
        renderer_map  = Gtk.CellRendererText()
        
        column1 = Gtk.TreeViewColumn("Path" , renderer_path, text=1)
        column2 = Gtk.TreeViewColumn("Map", renderer_map, text=2)
        
        
        self.treeview.append_column(column1)
        self.treeview.append_column(column2)
        
        self.grid.attach(self.treeview, 0,2,3,3)
        
    def on_image_button_clicked(self, widget):
        print("Hello World")
        Tk().withdraw() 
        self.mapname = askopenfilename() 
        print(self.mapname)
        if len(self.mapname) != 0:
            self.label.set_text('...' + self.mapname[len(self.mapname) - 10 :])
        self.testarray.append((9, self.mapname, self.mapname))
        print(self.testarray)
        self.liststore = self.make_list()
        self.treeview.set_model(self.liststore)
        
        #rect = self.grid.get_allocation()
        #self.grid.get_window().invalidate_rect(rect, False)
        
    def on_confirm_button_clicked(self, widget):
        print(self.entry.get_text())
        #self.label.set_text(self.entry.get_text())
        
    def make_list(self):
        """ do db things to generate list view """
        liststore = Gtk.ListStore(int, str, str)
        
        for line in self.testarray:
            liststore.append (list(line))
        return liststore
        
if __name__ == '__main__': 
    #a = Interface();
    
    win = Interface()
    win.connect("delete-event", Gtk.main_quit)
    win.show_all()
    Gtk.main()
