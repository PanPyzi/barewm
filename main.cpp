#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>







//READ CREATENOTIFY EVENT AND DEAL WITH THAT SHIT






/*void dowin (Display* dpy, Window win, int reg)
{
  Window root, parent;
  Window* children;
  int nchildren, i;

  XSelectInput (dpy, win, reg ? KeyReleaseMask|SubstructureNotifyMask : 0);
  XQueryTree (dpy, win, &root, &parent, &children, &nchildren);

  for (i = 0; i < nchildren; ++i)
  {
    dowin (dpy, children[i], reg);
  }

  XFree(children);
}

*/
void OnMapRequest(Display* dpy,const XMapRequestEvent& ev);
void OnCreateNotify(const XCreateWindowEvent& ev);
void OnDestroyNotify(const XDestroyWindowEvent& ev);
void sortWindows(Display* dpy,std::vector<Window> desktop_1,int desktop);

int main()
{
   int desktop;
    std::vector<Window> clients;
    std::vector<Window> desktop_1;
    std::vector<Window> desktop_2;

    Display*    dpy     = XOpenDisplay(0);
    Window      root_     = DefaultRootWindow(dpy);
    XEvent      ev;

    unsigned int    alt_modmask       = Mod1Mask;
    unsigned int    ignored_modmask   = 0; // stub
    KeyCode	    s_key		=XKeysymToKeycode(dpy,XK_S);
    KeyCode	    enter_key		=XKeysymToKeycode(dpy,XK_D);
    KeyCode         q_key      = XKeysymToKeycode(dpy,XK_Q);
    KeyCode         alt_key      = XKeysymToKeycode(dpy,XK_Alt_L);

   //ds        qqqqq dowin (dpy, win, True);
   XSelectInput(dpy, root_, SubstructureNotifyMask);
   XSync(dpy,false);

    XGrabKey (dpy, q_key, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);

   XGrabKey (dpy, s_key, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);
   XGrabKey (dpy, enter_key, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);

   XGrabServer(dpy);



                      Window returned_root, returned_parent;
                      Window* top_level_windows;
                      unsigned int num_top_level_windows;
                     XQueryTree(
                          dpy,
                          root_,
                          &returned_root,
                          &returned_parent,
                          &top_level_windows,
                          &num_top_level_windows);

                      //     ii. Frame each top-level window.
                      for (unsigned int i = 0; i < num_top_level_windows; ++i) {
                        //Frame(top_level_windows[i], true);
                        XSelectInput( dpy,top_level_windows[i], SubstructureRedirectMask | SubstructureNotifyMask);
                        XMapWindow(dpy, top_level_windows[i]);
                      }
                      //     iii. Free top-level window array.
                      XFree(top_level_windows);
                      //   e. Ungrab X server.
                      XUngrabServer(dpy);


   //Window XCreateSimpleWindow(dpy, root_, x, y, width, height, border_width, border, background);


   //XMapWindow(dpy, ev.window);

    while(true)
    {
      desktop =1;
      printf("R");
        ev.xkey.keycode = 0;
        ev.xkey.state = 0;
        ev.xkey.type = 0;

        XNextEvent(dpy, &ev);
        switch(ev.type)
        {
            case CreateNotify:
               OnCreateNotify(ev.xcreatewindow);
               clients.push_back(ev.xcreatewindow.window);
               if(desktop==1) desktop_1.push_back(ev.xcreatewindow.window);
               if(desktop==2) desktop_2.push_back(ev.xcreatewindow.window);
               printf("Window created: %lx, Client: %lx",ev.xcreatewindow.window, clients.back());
               break;
            case DestroyNotify:
                  OnDestroyNotify(ev.xdestroywindow);
            case KeyPress:
                printf ("Press %lx: d-%d\n", ev.xkey.window, ev.xkey.state, ev.xkey.keycode);
                break;

            case KeyRelease:
                printf ("Release %lx: %d-%d\n", ev.xkey.window, ev.xkey.state, ev.xkey.keycode);
                break;

            case MapNotify:
               OnMapRequest(dpy,ev.xmaprequest);
                printf ("Mapped %lx\n", ev.xmap.window);
      //          dowin (dpy, ev.xmap.window, True);
                break;

            case UnmapNotify:
                printf ("Unmapped %lx\n", ev.xunmap.window);
   //             dowin (dpy, ev.xunmap.window, False);
                break;

            default:
                printf ("Event type %d\n", ev.type);
                break;
        }
        if(ev.xkey.keycode==q_key && ev.xkey.type == KeyPress){
           break;
        }
	if(ev.xkey.keycode==enter_key && ev.xkey.type ==KeyPress){
	printf("Xterm started");

	system("xterm & disown");
   //sleep(3);
   printf("\nWindow: %d , subwindow: %d",ev.xkey.window,ev.xkey.subwindow);
   printf("\nWindow 1created: %lx\n",ev.xcreatewindow.window);

	}
   if(ev.xkey.keycode==s_key && ev.xkey.type ==KeyPress){
      sortWindows(dpy,desktop_1,desktop);

    }

 }
    XCloseDisplay(dpy);
    return 0;

}

void sortWindows(Display* dpy,std::vector<Window> desktop_1,int desktop){

   if (desktop==1){
      int height = 600/desktop_1.size();
      int width = 800/desktop_1.size();
      for(int i=0;i<desktop_1.size();i++){
         XResizeWindow(dpy, desktop_1.at(i), width, height);
         XMoveWindow(dpy, desktop_1.at(i),width*i,0);
      }
   }

}

void OnMapRequest(Display* dpy,const XMapRequestEvent& ev) {
  // 1. Frame or re-frame window.
  //Frame(e.window, false);
  // 2. Actually map window.
  XMapWindow(dpy, ev.window);
  return 0;
}


void OnCreateNotify(const XCreateWindowEvent& ev){return 0;}
void OnDestroyNotify(const XDestroyWindowEvent& ev){return 0;}
