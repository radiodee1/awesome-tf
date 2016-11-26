
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
        
        self.originalarray =  [(0,"img/map.png" , "first map"),
                        (1,"img/smallmap.png","second map"),
                        (2,"img/maze.png", "maze"),
                        (5,"img/notthere.png","not there")]
        
        self.testarray =     [(0,"img/map.png" , "first map"),
                        (1,"img/smallmap.png","second map"),
                        (2,"img/maze.png", "maze"),
                        (5,"img/notthere.png","not there")]
        
        Gtk.Window.__init__(self, title="Maps")
        self.set_border_width(10)

        self.grid = Gtk.Grid()
        self.add(self.grid)
        
        self.button = Gtk.Button(label="Load PNG Image")
        self.button.connect("clicked", self.on_image_button_clicked)
        self.grid.add(self.button)
        
        self.label = Gtk.Label("display-name")
        self.grid.attach(self.label, 0, 1, 1, 1)
        
        self.entry = Gtk.Entry()
        self.entry.set_text("")
        self.entry.connect("activate", self.enter_callback)
        self.grid.attach(self.entry, 1, 1, 1, 1)
        
        self.confirm = Gtk.Button(label="Save/Send Image")
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
        
        self.select = self.treeview.get_selection()
        self.select.connect("changed", self.on_selection_changed)
        
        self.grid.attach(self.treeview, 0,2,3,3)
        
    def enter_callback(self, widget):
        print (self.entry.get_text())
        self.label.set_text(self.entry.get_text())
        
    def on_selection_changed(self, widget):
        model, treeiter = widget.get_selected()
        if treeiter != None:
            print("selected", model[treeiter][0] )
            self.mapname = self.originalarray[int(model[treeiter][0])][1]
            print (self.mapname)
        
    def on_image_button_clicked(self, widget):
        print("Hello World")
        Tk().withdraw() 
        self.mapname = askopenfilename() 
        print(self.mapname)
        if len(self.mapname) != 0:
            self.label.set_text('...' + self.mapname[ - 10 :])
            self.testarray.append((9,'...'+ self.mapname[-10: ],'...'+ self.mapname[-10:]))
            self.originalarray.append( (9,  self.mapname, self.mapname ) )
        print(self.testarray)
        print(self.originalarray)
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
