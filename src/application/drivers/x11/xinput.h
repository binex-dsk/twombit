#ifndef XINPUT_H
#define XINPUT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>

#include <string.h>
#include <string>
using namespace std;
#include <dataTypes.h>
#include <helper.h>
#include "../drvinput.h"
#include "../../inputmanager/inputmap.h"

class SInput; //use sdl for gamepads

class XInput : public DrvInput
{
    #include "xlibkeys.h"
    static string keyname[256];

    SInput* sinput;
    Display* display;
    Window rootwindow;
    unsigned relativex, relativey;

    bool mouseacquired;
    unsigned long long handle;

    void term();

public:
    XInput();
    ~XInput();
    template <typename T> void init(T handle) { init((unsigned long long)handle); }
    void init(unsigned long long _handle);
    void poll(i16* table);
    string getKeyName(unsigned element);
    string getJoyName(unsigned element);

    void m_acquire();
    void m_unacquire();
    bool m_isAcquired() {
        return mouseacquired;
    }
};

#endif // XINPUT_H
