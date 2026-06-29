#include <X11/extensions/scrnsaver.h>
#include <X11/Xlib.h>

#include <iostream>
#include <thread>
#include <chrono>

/*
 * first i thought about using the below mentioned function but then i just discovered that i can just call XScreenSaverQuerryInfo(), but then i had to read 40 million man pages, it was cool
 
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
*/

/*
 *from the XScreenSaverQueryInfo manpage;
        SYNOPSIS                                                                                                                                   git:main*
               #include <X11/extensions/scrnsaver.h>

               typedef struct {
                   Window window;                // screen saver window 
                   int state;                    // ScreenSaver{Off,On,Disabled} 
                   int kind;                     // ScreenSaver{Blanked,Internal,External} 
                   unsigned long til_or_since;   // milliseconds 
                   unsigned long idle;           // milliseconds 
                   unsigned long eventMask;      // events 
               } XScreenSaverInfo;
            
            and that  "XScreenSaverQueryInfo returns information about the current state of the screen server in saver_info and a non-zero value is returned.  If  the  extension is not supported, saver_info is not changed and 0 is returned."

       but now wtf is is Status??

       rg "typedef.*Status" /usr/include/X11                                                                                                      git:main*
            /usr/include/X11/Xutil.h
            231:typedef struct _XComposeStatus {

            /usr/include/X11/Xcms.h
            208:typedef Status (*XcmsCompressionProc)(              // Gamut Compression Proc 
            216:typedef Status (*XcmsWhiteAdjustProc)(              // White Point Adjust Proc 
            243:typedef Status (*XcmsScreenInitProc)(       // Screen Initialization Proc 
            261:typedef Status (*XcmsConversionProc)(XcmsCCC, XcmsColor *, XcmsColor *,
            266:typedef Status (*XcmsConversionProc)(XcmsCCC, XcmsColor *, unsigned int,
            272:typedef Status (*XcmsDDConversionProc)( // using device-dependent version 
            279:typedef Status (*XcmsDIConversionProc)( // using device-independent version 

            /usr/include/X11/Xlib.h
            1353:typedef struct _XIMStatusDrawCallbackStruct {

            /usr/include/X11/extensions/XKBsrv.h
            330:typedef int Status;

            /usr/include/X11/extensions/xf86dgaproto.h
            226:typedef struct _XDGAGetViewportStatus {

        so Status is just an int, bruh. and it says if it fails it returns 0 otherwise the idle time in milliseconds
  */

constexpr int timeout_in_ms = 600000; // ofc \pm sleep_for_secs
constexpr int sleep_for_secs = 5;

int main() {
    // so connect to the xserver first (from man XOpenDisplay)
    Display *display = XOpenDisplay(nullptr); // that points to display struct and if nullptr then it fills it up with the current default display
    // when done we use XCloseDisplay(display) to close
    
    if (!display) {
        std::cerr << "failed to connected to the Xserver \n";
    }

    XScreenSaverInfo *info = XScreenSaverAllocInfo(); // this is an XID which is also long long

    if (!info) {
        std::cerr << "failed to alloc XscreensaverInfo \n";
        XCloseDisplay(display);
    }

    // get the root window which is a Drawable which is just an XID which in turn is a long long
    Drawable root_window = DefaultRootWindow(display);
    bool locked = false; // this flag will help in not runing the script again if screen is already locked

    while (true) {
        Status ok = XScreenSaverQueryInfo(display, root_window, info); // this returns 0 if it fails for some reason
        
        if (!ok) {
            std::cerr << "something went wrong! \n";
        }

        if (info->idle > timeout_in_ms && !locked) {
            std::system("/home/ludvary/.config/eww/scripts/i3lock-widgets");
            locked = true;
        }

        if (info->idle < timeout_in_ms) {
            locked = false;
        }

        std::this_thread::sleep_for(std::chrono::seconds(sleep_for_secs));
    }

    XFree(info);
    XCloseDisplay(display);


    return 0;
}
