#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char *argv[]){
	//Get system window
	Display *dpy;
	Window root_window;

	dpy = XOpenDisplay(0);
	root_window = XRootWindow(dpy, 0);
	XSelectInput(dpy, root_window, KeyReleaseMask);
	XWarpPointer(dpy, 			// display,
				 None,	 		// src_w,
				 root_window, 	// dest_w,
				 0, 0, 			// src_x, src_y,
				 0, 0, 			// src_width, src_height,
				 20, 20 			// dest_x, dest_y
				 );			

	XFlush(dpy);

	return 0;
}
