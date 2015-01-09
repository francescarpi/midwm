#!/usr/bin/env python
# encoding: utf-8
# --------------------------------------------------------------------------

import gtk

if __name__ == "__main__":
    statusIcon = gtk.StatusIcon()
    window = gtk.OffscreenWindow()
    window.add(gtk.Label(' 1.23 '))

    def draw_complete_event(window, event, statusIcon=statusIcon):
        statusIcon.set_from_pixbuf(window.get_pixbuf())

    window.connect('damage-event', draw_complete_event)
    window.show_all()

    gtk.main()
