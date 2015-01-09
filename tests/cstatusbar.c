/* Compilación gcc cstatusbar.c -o cstatusbar -g -ggdb -lX11 -lXft -L/usr/lib/X11 -lXinerama -I/usr/include/X11 -I/usr/include/freetype2 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

static Display *dpy;
static int screen;
static Window root;
char status[256];
static int sw, sh; // Ancho y alto de la pantalla

typedef struct {
	Display *dpy;
	int ascent;
	int descent;
	unsigned int h;
	XftFont *xfont;
} Fnt;

typedef struct {
	unsigned long pix;
	XftColor rgb;
} Clr;

typedef struct {
	Clr *fg;
	Clr *bg;
	Clr *border;
} ClrScheme;

typedef struct {
	unsigned int w, h;
	Display *dpy;
	int screen;
	Window root;
	Drawable drawable;
	GC gc;
	ClrScheme *scheme;
	Fnt *font;
} Drw;

static Drw *drw;

static Bool gettextprop(Window w, Atom atom, char *text, unsigned int size);
static void loadstatus(void);
static void cleanup(void);
Drw *drw_create(Display *dpy, int screen, Window win, unsigned int w, unsigned int h);
static void drwstatus(void);
void drw_text_clr(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text, int invert);

// Función main, principal
int
main(int argc, int *argv[]) {
    loadstatus();
    cleanup();
    drwstatus();
    return EXIT_SUCCESS;
} 

void
drw_text_clr(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text, int invert) {

	Colormap cmap;
	Visual *vis;
	XftDraw *d;

	cmap = DefaultColormap(drw->dpy, drw->screen);
	vis = DefaultVisual(drw->dpy, drw->screen);

	d = XftDrawCreate(drw->dpy, drw->drawable, vis, cmap);
	XftDrawStringUtf8(d, &drw->scheme->fg->rgb, drw->font->xfont, x, y, (XftChar8 *)buf, len);
	XftDrawDestroy(d);

}

// Dibujamos una barra de estado
void
drwstatus(void) {
    sw = DisplayWidth(dpy, screen);
    sh = DisplayHeight(dpy, screen);
    drw = drw_create(dpy, screen, root, sw, sh);
	drw_text_clr(drw, 0, 10, 200, 20, status, 0);

}

Drw *
drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h) {
	Drw *drw = (Drw *)calloc(1, sizeof(Drw));
	if(!drw)
		return NULL;
	drw->dpy = dpy;
	drw->screen = screen;
	drw->root = root;
	drw->w = w;
	drw->h = h;
	drw->drawable = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
	drw->gc = XCreateGC(dpy, root, 0, NULL);
	XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);
	return drw;
}

// Recupera el valor de la propiedad XA_WM_NAME, es decir el contenido
// de la barra de estado
void
loadstatus(void) {
    dpy = XOpenDisplay(NULL);
    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);
    gettextprop(root, XA_WM_NAME, status, sizeof(status)); 
}

// Nos facilita la recuperación de valores de variables de X11
Bool
gettextprop(Window w, Atom atom, char *text, unsigned int size) {
    char **list = NULL;
    int n;
    XTextProperty name;

    if(!text || size == 0)
        return False;
    text[0] = '\0';
    XGetTextProperty(dpy, w, &name, atom);
    if(!name.nitems)
        return False;
    if(name.encoding == XA_STRING)
        strncpy(text, (char *)name.value, size - 1);
    else {
        if(XmbTextPropertyToTextList(dpy, &name, &list, &n) >= Success && n > 0 && *list) {
            strncpy(text, *list, size - 1);
            XFreeStringList(list);
        }
    }
    text[size - 1] = '\0';
    XFree(name.value);
    return True;
}

// Limpia memoria
void
cleanup(void) {
	/* XFreePixmap(drw->dpy, drw->drawable); */
	/* XFreeGC(drw->dpy, drw->gc); */
	free(drw);
    XSync(dpy, False);
}
