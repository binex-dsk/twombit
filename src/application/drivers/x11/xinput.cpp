#include "xinput.h"
#include "../sdl/sinput.h"

#include <GL/glx.h>

XInput::XInput()
{
    sinput = new SInput();
    display = XOpenDisplay(0);
    handle = 0;
}

XInput::~XInput() {
    term();
    delete sinput;
}

void XInput::m_acquire() {
    if(m_isAcquired()) return;

    if(XGrabPointer(display, handle, True, 0, GrabModeAsync, GrabModeAsync,
    rootwindow, 0, CurrentTime) == GrabSuccess) {
        XFixesHideCursor(display, rootwindow);
        XGrabButton(display,AnyButton, AnyModifier, rootwindow, false,
        0, GrabModeAsync, GrabModeAsync, rootwindow, 0);
        mouseacquired = true;
        return;
    }
    mouseacquired = false;
}

void XInput::m_unacquire() {
    if(m_isAcquired()) {
        XUngrabPointer(display, CurrentTime);
        XUngrabButton(display, AnyButton, AnyModifier, rootwindow);
        XFixesShowCursor(display, rootwindow);
        mouseacquired = false;
    }
}

string XInput::getJoyName(unsigned element) {
    return sinput->getJoyName(element);
}

string XInput::getKeyName(unsigned element) {
    return keyname[element];
}

void XInput::poll(i16* table) {
    memset(table, 0, InputMap::ElementCounter * sizeof(i16));

    x_poll(display, table);
    table += InputMap::Keyboard::elements;

    Window root_return, child_return;
    int root_x_return = 0, root_y_return = 0;
    int win_x_return = 0, win_y_return = 0;
    unsigned int mask_return = 0;

    XQueryPointer(display, rootwindow,
        &root_return, &child_return, &root_x_return, &root_y_return,
        &win_x_return, &win_y_return, &mask_return);

    table[InputMap::Mouse::x] = (i16)(root_x_return - relativex);
    table[InputMap::Mouse::y] = (i16)(root_y_return - relativey);

    relativex = root_x_return;
    relativey = root_y_return;

    table[InputMap::Mouse::button + 0] = (bool)(mask_return & Button1Mask);
    table[InputMap::Mouse::button + 1] = (bool)(mask_return & Button2Mask);
    table[InputMap::Mouse::button + 2] = (bool)(mask_return & Button3Mask);
    table[InputMap::Mouse::button + 3] = (bool)(mask_return & Button4Mask);
    table[InputMap::Mouse::button + 4] = (bool)(mask_return & Button5Mask);

    table += InputMap::Mouse::elements;
    sinput->pollJoypad(table);
}

void XInput::init(unsigned long long _handle) {
    handle = _handle;
    sinput->init();

    rootwindow = DefaultRootWindow(display);
    x_init(display);

    mouseacquired = false;
    relativex = 0;
    relativey = 0;
}

void XInput::term() {
    XCloseDisplay(display);
}

string XInput::keyname[] = {"",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
    "Pause", "Tilde",
    "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Num0",
    "Dash", "Equal", "Backspace",
    "Insert", "Delete", "Home", "End", "PageUp", "PageDown",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
    "LeftBracket", "RightBracket", "Backslash", "Semicolon", "Apostrophe", "Comma", "Period", "Slash",
    "Keypad1", "Keypad2", "Keypad3", "Keypad4", "Keypad5", "Keypad6", "Keypad7", "Keypad8", "Keypad9", "Keypad0",
    "Separator", "Enter", "Add", "Subtract", "Multiply", "Divide",
    "Up", "Down", "Left", "Right",
    "Tab", "Return", "Spacebar", "Menu",
    "LeftShift", "RightShift", "LeftControl", "RightControl", "LeftAlt", "RightAlt"
};
