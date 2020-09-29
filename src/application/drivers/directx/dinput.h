#ifndef DINPUT_H
#define DINPUT_H

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <dataTypes.h>
#include <string>
#include "../../inputmanager/inputmap.h"
#include "../drvinput.h"
using namespace std;

class DInput : public DrvInput {
public:
    template <typename T> void init(T handle) { init(handle); }
    void init(HWND handle);
    void term();
    void poll(i16* i_map);
    void m_acquire();
    void m_unacquire();
    bool m_isAcquired() { return m_acquired; }

private:
    HWND hwnd;
    LPDIRECTINPUT8 din;
    LPDIRECTINPUTDEVICE8 din_key, din_joy[InputMap::Joypad::count], din_mouse;
    string device_name[InputMap::Joypad::count];
    bool m_acquired;

    u8 din_joy_count;
    bool enum_joypads(const DIDEVICEINSTANCE*);
    static BOOL CALLBACK EnumJoypadsCallback(const DIDEVICEINSTANCE*, void*);

public:
    string getJoyName(unsigned element) {
        return device_name[element];
    }

    string getKeyName(unsigned element) {
        if (element & 0x80) element += 0x80;
        if (element == 0x45) element = 0x145; //Pause <> NumPad
        else if (element == 0x145) element = 0x45;

        if (element == 0x135) return "/ (Numpad)";
        else if (element == 0x37) return "* (Numpad)";

        CHAR keyText[100];
        GetKeyNameText(element << 16, keyText, 100);

        return (string)keyText;
    }

    DInput();
    ~DInput();
};

#endif // DINPUT_H
