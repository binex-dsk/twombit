#ifndef CINPUT_H
#define CINPUT_H

#include <string.h>
#include <string>
using namespace std;
#include <dataTypes.h>
#include <helper.h>
#include "../drvinput.h"
#include "../../inputmanager/inputmap.h"

class SInput; //use sdl for gamepads

class CInput : public DrvInput
{
    enum XScancode { UNMAPPED,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Pause, Tilde,
        Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
        Dash, Equal, Backspace,
        Insert, Delete, Home, End, PageUp, PageDown,
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        LeftBracket, RightBracket, Backslash, Semicolon, Apostrophe, Comma, Period, Slash,
        Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9, Keypad0,
        Separator, Enter, Add, Subtract, Multiply, Divide,
        Up, Down, Left, Right,
        Tab, Return, Spacebar, Menu,
        LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt
    };

    static string keyname[256];

    SInput* sinput;
    unsigned relativex, relativey;

    unsigned long long handle;

    void term();
    void pollKeyboard(i16* table);
    void pollMouse(i16* table);

public:
    CInput();
    ~CInput();
    template <typename T> void init(T handle) { init((unsigned long long)handle); }
    void init(unsigned long long _handle);

    void poll(i16* table);
    string getKeyName(unsigned element);
    string getJoyName(unsigned element);

    void m_acquire();
    void m_unacquire();
    bool m_isAcquired();
};

#endif // CINPUT_H
