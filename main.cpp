#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdbool.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <typeinfo>


struct client{ Window win; int desktop;bool visible;};


void OnMapRequest(Display* dpy,const XMapRequestEvent& ev);
void OnCreateNotify(const XCreateWindowEvent& ev);
void OnDestroyNotify(const XDestroyWindowEvent& ev);
void SortWindows(Display* dpy,std::vector<client> clients,int desktop, int sort, int desktopWin);
void ChangeVDesktop(Display* dpy,std::vector<client> clients,int desktop, int sort);
void KillWindow(Display* dpy, Window w);


int main()
{
   int desktopWin[10];
   for(int i =0;i<10;i++) desktopWin[i]=0;
   int desktop = 1;//default virtual desktop
   int sort = 1;//default tiling 0-floating, 1-simple tiling
   int sorttypes=2;
    std::vector<client> clients;

    Display*    dpy     = XOpenDisplay(0);
    Window      root_     = DefaultRootWindow(dpy);
    XEvent      ev;

    unsigned int    alt_modmask       = Mod1Mask;
    unsigned int    ignored_modmask   = 0; // stub
    KeyCode	    key_1		=XKeysymToKeycode(dpy,XK_1);
    KeyCode	    key_2		=XKeysymToKeycode(dpy,XK_2);
    KeyCode	    s_key		=XKeysymToKeycode(dpy,XK_S);
    KeyCode	    enter_key		=XKeysymToKeycode(dpy,XK_D);
    KeyCode         q_key      = XKeysymToKeycode(dpy,XK_Q);
    KeyCode         alt_key      = XKeysymToKeycode(dpy,XK_Alt_L);
    //Window menu=XCreateSimpleWindow(dpy, root_, 0, 0, 10, 10, 10, 0x0000ff, 0x00000f);
   // XMapWindow(dpy, menu);

   //ds        qqqqq dowin (dpy, win, True);
   XSelectInput(dpy, root_, SubstructureNotifyMask);
   XSync(dpy,false);

    XGrabKey (dpy, q_key, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);

   XGrabKey (dpy, s_key, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);
   XGrabKey (dpy, enter_key, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);
   XGrabKey (dpy, key_1, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);
   XGrabKey (dpy, key_2, alt_modmask | ignored_modmask, root_, True, GrabModeAsync, GrabModeAsync);


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

        ev.xkey.keycode = 0;
        ev.xkey.state = 0;
        ev.xkey.type = 0;

        XNextEvent(dpy, &ev);
        switch(ev.type)
        {
            case CreateNotify:
               OnCreateNotify(ev.xcreatewindow);
               clients.push_back(client());
               clients.back().win=ev.xcreatewindow.window;
               clients.back().desktop=desktop;
               desktopWin[desktop]++;
               printf("desktopWin: %d",desktopWin[desktop]);
               //clients.back().visible=true;


               //printf("Window created: %lx, Client: %lx",ev.xcreatewindow.window, clients.back());

               break;
            case DestroyNotify:
                  OnDestroyNotify(ev.xdestroywindow);
                  SortWindows(dpy,clients,desktop,sort,desktopWin[desktop]);
            case KeyPress:
                printf ("Press %lx: %lx-%lx\n", ev.xkey.root, ev.xkey.window, ev.xkey.subwindow);
                break;

            case KeyRelease:
                printf ("Release %lx: %d-%d\n", ev.xkey.window, ev.xkey.state, ev.xkey.keycode);
                break;

            case MapNotify:
               OnMapRequest(dpy,ev.xmaprequest);
                printf ("Mapped %lx\n", ev.xmap.window);
                SortWindows(dpy,clients,desktop,sort,desktopWin[desktop]);
      //          dowin (dpy, ev.xmap.window, True);
                break;

            case UnmapNotify:
               // printf ("Unmapped %lx\n", ev.xunmap.window);
                SortWindows(dpy,clients,desktop,sort,desktopWin[desktop]);
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

        ///////////////////////////////////////////////////////////
        ////                                                   ////
        ////            !! Switch ifs to cases !!              ////
        ////                                                   ////
        ///////////////////////////////////////////////////////////

       if(ev.xkey.keycode==key_1 && ev.xkey.type == KeyPress){
           desktop=1;
           printf("desktop1\n");
           //ChangeVDesktop(dpy,clients,desktop,sort);
           SortWindows(dpy,clients,desktop,sort,desktopWin[desktop]);

         }
       if(ev.xkey.keycode==key_2 && ev.xkey.type == KeyPress){
           desktop=2;
           printf("desktop2\n");
           //ChangeVDesktop(dpy,clients,desktop,sort);
           SortWindows(dpy,clients,desktop,sort,desktopWin[desktop]);
         }
        if(ev.xkey.keycode==q_key && ev.xkey.type == KeyPress){
           for(long unsigned int i=0;i<clients.size();i++){
             if(clients[i].win==ev.xkey.subwindow){
                  printf ("SEARCHED FOR: %lx\n", ev.xkey.subwindow);
                  printf ("FOUND: %lx\n", clients[i].win);
                  printf ("Type: %s\n", typeid(clients[i].win).name());
                  if(typeid(clients[i].win).name()==typeid(root_).name()) KillWindow(dpy, clients[i].win);
                 clients.erase(clients.begin() + i);
                 break;
               }
            }
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
      SortWindows(dpy,clients,desktop,sort,desktopWin[desktop]);

    }

 }
    XCloseDisplay(dpy);
    return 0;

}

void SortWindows(Display* dpy,std::vector<client> clients,int desktop, int sort,int desktopWin){
int rows=2,k=0,d=0;
Screen *screen= XDefaultScreenOfDisplay(dpy);
int height =XHeightOfScreen(screen);
int width =XWidthOfScreen(screen);
long unsigned int size = clients.size();
printf("Desktop= %d\n",desktop);
if(size==0) return;
if (sort==1){
       if(desktopWin!=0)width /=desktopWin;
      for(long unsigned int i=0;i<size;i++){
         if(clients[k].desktop==desktop){
            XMoveResizeWindow(dpy, clients[k].win, width*d, 0, width, height);
            printf(" sort=1 Win: %lx, visible: %d, W: %ld, H: %d, w: %d, h %d\n",clients.at(k).win,clients.at(k).visible, width*d, height, width, height);

            if(clients[k].visible!=true){
                XMapWindow(dpy,clients[k].win);
                clients[k].visible=true;
             }
            d++;
            printf("D: %d \n",d);
         }
         else{
            XUnmapWindow(dpy,clients[k].win);
            clients[k].visible=false;
         }
         printf(" sort=1 Win: %lx, visible: %d\n",clients.at(k).win,clients.at(k).visible);
         k++;
      }
      printf("\nSorted \n");
      return;
   }

   if (sort==2){

      if(desktopWin%2==0){
         if(desktopWin>=2||desktopWin<=4){
            width /=(desktopWin/2);
            height /=2;
            rows=2;
            }

         for(int j=0;j<rows;j++){///////ADD FUNCTION FOR WHEN NUM OF WINDOWS ISNT 2%==0
            for(long unsigned int i=0 ; i< desktopWin/rows ;i++){
               if(clients.at(k).desktop==desktop){
                  if(j==0){
                     XMoveResizeWindow(dpy, clients.at(k).win, width*(d), height*j, width, height);
                     printf("sort=2 win: %ld, W: %ld, H: %d, w: %d, h %d\n",clients.at(d+1).win,width*d, height*j, width, height);
                  }else{
                     XMoveResizeWindow(dpy, clients.at(k).win, width*(d-j), height*j, width, height);

                     printf("sort=2 win: %ld, W: %ld, H: %d, w: %d, h %d\n",clients.at(k).win,width*(d-j), height*j, width, height);
                  }
                  if(clients[k].visible!=true){
                     XMapWindow(dpy,clients[k].win);
                     clients[k].visible=true;
                  }
                  d++;
                  printf("D: %d \n",d);
               }
               else{
                  XUnmapWindow(dpy,clients[k].win);
                  clients[k].visible=false;
               }
               k++;
            }
         }
      }else{
         if(desktopWin>1 && desktopWin<5){
            width /=desktopWin-1;
            height /=rows;

         }
         if(desktopWin>=5){
            int g=2;
            for(long unsigned int i=5;i<desktopWin;i++)if(i%2!=0)g++;
            width /=desktopWin-g;
            height /=rows;
         }
         for(int j=0;j<rows;j++){///////ADD FUNCTION FOR WHEN NUM OF WINDOWS ISNT 2%==0
            for(long unsigned int i=0 ; i< desktopWin/(rows);i++){
                  if(clients.at(i).desktop==desktop){
                     if(j==0){
                        XMoveResizeWindow(dpy, clients.at(k).win, width*(d), height*j, width, height);
                        printf("sort=2/3 win: %lx, W: %ld, H: %d, w: %d, h %d\n",clients.at(k).win,width*(d), height*j, width, height);
                     }else{
                        XMoveResizeWindow(dpy, clients.at(k).win, width*(j+1), height*j, width, height);
                        printf("sort=2/3 win: %lx, W: %ld, H: %d, w: %d, h %d, visible: %d\n",clients.at(k).win,width*(d-j), height*j, width, height,clients[k].visible);
                     }

                     if(clients[k].visible!=true){
                        XMapWindow(dpy,clients[k].win);
                        clients[k].visible=true;
                     }
                     d++;
                     printf("D: %d \n",d);
                  }
                  else{
                     XUnmapWindow(dpy,clients[k].win);
                     clients[k].visible=false;
                  }
                  printf("sort=2/3 win: %lx,visible: %d\n",clients.at(k).win,clients[k].visible);

                  k++;
            }

         }
         bool hit=false;
            int b =size;
         while(!hit){
            if(clients[b].desktop==desktop){
               XMoveResizeWindow(dpy, clients[b].win, width*((size/rows)), 0, width, height*2);
               printf("sort=2/32 win: %lx, W: %ld, H: %d, w: %d, h %d\n",clients[b].win,width*((size/rows)), 0, width, height*2);
               hit=true;
               if(clients[b].visible!=true){
                  XMapWindow(dpy,clients[b].win);
                  clients[b].visible=true;
                  printf("sort=2/23 win: %lx,visible: %d\n",clients[b].win,clients[b].visible);
               }
            }
            else b--;
         }
      }
      printf("\nSorted \n");
      return;
   }

}
void OnMapRequest(Display* dpy,const XMapRequestEvent& ev) {
  // 1. Frame or re-frame window.
  //Frame(e.window, false);
  // 2. Actually map window.
  XMapWindow(dpy, ev.window);
  return ;
}

void KillWindow(Display* dpy, Window w){
   printf("Kill %lx\n",w);
   XKillClient(dpy, w);

}
void ChangeVDesktop(Display* dpy,std::vector<client> clients,int desktop, int sort){
   long unsigned int size = clients.size();
   for(long unsigned int i=0; i<size;i++){
      if(clients[i].desktop==desktop){
         XMapWindow(dpy, clients[i].win);
         clients[i].visible=true;
      }
      else{
         XUnmapWindow(dpy, clients[i].win);
         clients[i].visible=false;

      }

   }

}

void OnCreateNotify(const XCreateWindowEvent& ev){return;}
void OnDestroyNotify(const XDestroyWindowEvent& ev){return;}
