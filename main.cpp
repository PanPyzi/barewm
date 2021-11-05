#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <unistd.h>






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
void sortWindows(Display* dpy,std::vector<Window> desktop_1,int desktop, int sort);

int main()
{
   int desktop = 1;//default virtual desktop
   int sort = 2;//default tiling 0-floating, 1-simple tiling
   int sorttypes=2;
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
               //printf("Window created: %lx, Client: %lx",ev.xcreatewindow.window, clients.back());

               break;
            case DestroyNotify:
                  OnDestroyNotify(ev.xdestroywindow);
                  sortWindows(dpy,desktop_1,desktop,sort);
            case KeyPress:
                printf ("Press %lx: d-%d\n", ev.xkey.window, ev.xkey.state, ev.xkey.keycode);
                break;

            case KeyRelease:
                printf ("Release %lx: %d-%d\n", ev.xkey.window, ev.xkey.state, ev.xkey.keycode);
                break;

            case MapNotify:
               OnMapRequest(dpy,ev.xmaprequest);
                printf ("Mapped %lx\n", ev.xmap.window);
                sortWindows(dpy,desktop_1,desktop,sort);
      //          dowin (dpy, ev.xmap.window, True);
                break;

            case UnmapNotify:
               // printf ("Unmapped %lx\n", ev.xunmap.window);
                sortWindows(dpy,desktop_1,desktop,sort);
   //             dowin (dpy, ev.xunmap.window, False);
                break;
           case ConfigureNotify:
                  printf ("Configured %lx\n", ev.xconfigurerequest.window);
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
	//printf("Xterm started");

	system("xterm & disown");
   //sleep(3);
   //printf("\nWindow: %d , subwindow: %d",ev.xkey.window,ev.xkey.subwindow);
   //printf("\nWindow 1created: %lx\n",ev.xcreatewindow.window);

	}
   if(ev.xkey.keycode==s_key && ev.xkey.type ==KeyPress){
   if(sort<sorttypes)sort++;
   else sort=1;
      sortWindows(dpy,desktop_1,desktop,sort);

    }

 }
    XCloseDisplay(dpy);
    return 0;

}

void sortWindows(Display* dpy,std::vector<Window> desktop_1,int desktop, int sort){
int rows=2,k=0;
Screen *screen= XDefaultScreenOfDisplay(dpy);
int height =XHeightOfScreen(screen);
int width =XWidthOfScreen(screen);

//if(desktop_1.size()==1)XMoveResizeWindow(dpy, desktop_1.at(0), 0, 0, 400, 300);
//if(desktop_1.size()==2)XMoveResizeWindow(dpy, desktop_1.at(1), 400, 0, 400, 300);
//if(desktop_1.size()==3)XMoveResizeWindow(dpy, desktop_1.at(2), 0, 300, 400, 300);
//if(desktop_1.size()==4)XMoveResizeWindow(dpy, desktop_1.at(3), 400, 300, 400, 300);

if (desktop==1&&sort==1){
       width /=desktop_1.size();
      for(int i=0;i<desktop_1.size();i++){

         XMoveResizeWindow(dpy, desktop_1.at(i), width*i, 0, width, height);
         printf(" sort=1 W: %d, H: %d, w: %d, h %d\n",width*(i), 0, width, height);
      }
   }

   if (desktop==1&&sort==2){

      if(desktop_1.size()%2==0){
         if(desktop_1.size()>=2||desktop_1.size()<=4){
            width /=(desktop_1.size()/2);
            height /=2;
            rows=2;
            }

         for(int j=0;j<rows;j++){///////ADD FUNCTION FOR WHEN NUM OF WINDOWS ISNT 2%==0
            for(int i=0 ; i< desktop_1.size()/rows ;i++){
               if(j==0){
                  XMoveResizeWindow(dpy, desktop_1.at(k), width*(i), height*j, width, height);
                  printf("sort=2 win: %lx, W: %d, H: %d, w: %d, h %d\n",desktop_1.at(k),width*(i), height*j, width, height);
               }else{
                  XMoveResizeWindow(dpy, desktop_1.at(k), width*(i-j+1), height*j, width, height);

               printf("sort=2 win: %lx, W: %d, H: %d, w: %d, h %d\n",desktop_1.at(k),width*(i-j+1), height*j, width, height);
               }

            k++;
            }

         }
      }else{
         if(desktop_1.size()>1 && desktop_1.size()<5){
            width /=desktop_1.size()-1;
            height /=rows;

         }
         if(desktop_1.size()>=5){
            int g=2;
            for(int i=5;i<desktop_1.size();i++)if(i%2!=0)g++;
            width /=desktop_1.size()-g;
            height /=rows;
         }
         for(int j=0;j<rows;j++){///////ADD FUNCTION FOR WHEN NUM OF WINDOWS ISNT 2%==0
            for(int i=0 ; i< desktop_1.size()/(rows);i++){
               if(j==0){
                  XMoveResizeWindow(dpy, desktop_1.at(k), width*(i), height*j, width, height);
                  printf("sort=2/3 win: %lx, W: %d, H: %d, w: %d, h %d\n",desktop_1.at(k),width*(i), height*j, width, height);
               }else{
                  XMoveResizeWindow(dpy, desktop_1.at(k), width*(i-j+1), height*j, width, height);

               printf("sort=2/3 win: %lx, W: %d, H: %d, w: %d, h %d\n",desktop_1.at(k),width*(i-j+1), height*j, width, height);
               }

            k++;
            }

         }
         XMoveResizeWindow(dpy, desktop_1.back(), width*((desktop_1.size()/rows)), 0, width, height*2);
         printf("sort=2/3 win: %lx, W: %d, H: %d, w: %d, h %d\n",desktop_1.at(k),width*((desktop_1.size()/rows)), 0, width, height*2);
      }
   }
   printf("\nSorted \n");
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
