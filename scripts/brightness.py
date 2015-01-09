#!/usr/bin/env python
# encoding: utf-8
# --------------------------------------------------------------------------

import subprocess
import gtk


class SystrayIconApp:
    def __init__(self):
        self.tray = gtk.StatusIcon()
        # self.tray.set_from_stock(gtk.STOCK_ABOUT)
        self.tray.set_from_file('/usr/share/icons/gnome/16x16/devices/video-display.png')
        self.tray.connect('popup-menu', self.on_right_click)
        self.tray.set_tooltip(('Indicador de brillo'))
        self.steps = ['15%', '50%', '100%']

    def on_right_click(self, icon, event_button, event_time):
        self.make_menu(event_button, event_time)

    def make_menu(self, event_button, event_time):
        menu = gtk.Menu()

        # brightness
        bright = gtk.MenuItem('Brillo actual: %s' % self.get_brightness())
        bright.show()
        menu.append(bright)

        sep = gtk.SeparatorMenuItem()
        sep.show()
        menu.append(sep)

        for step in self.steps:
            mstep = gtk.MenuItem(step)
            mstep.show()
            menu.append(mstep)
            mstep.connect('activate', self.set_brightness, step)

        # add quit item
        sep = gtk.SeparatorMenuItem()
        sep.show()
        menu.append(sep)

        quit = gtk.MenuItem('Salir')
        quit.show()
        menu.append(quit)
        quit.connect('activate', gtk.main_quit)

        menu.popup(None, None, gtk.status_icon_position_menu, event_button, event_time, self.tray)

    def get_brightness(self):
        ps = subprocess.Popen(['xbacklight'], shell=True, stdout=subprocess.PIPE)
        output = float(ps.communicate()[0])
        return '%.2f%%' % output

    def set_brightness(self, widget, *data):
        subprocess.call(['xbacklight', '-set', data[0]], shell=False)

if __name__ == "__main__":
    SystrayIconApp()
    gtk.main()
