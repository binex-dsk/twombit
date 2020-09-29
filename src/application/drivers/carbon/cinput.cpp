#include "cinput.h"
#include "../sdl/sinput.h"
#include <Carbon/Carbon.h>


CInput::CInput()
{
    sinput = new SInput();
    handle = 0;
    relativex = relativey = 0;
}

CInput::~CInput() {
    term();
    delete sinput;
}

bool CInput::m_isAcquired() {
    return !CGCursorIsVisible();
}


void CInput::m_acquire() {
    if(m_isAcquired()) return;
    CGAssociateMouseAndMouseCursorPosition(false);
    CGDisplayHideCursor(0);

}

void CInput::m_unacquire() {
    if(m_isAcquired()) {
        CGAssociateMouseAndMouseCursorPosition(true);
        CGDisplayShowCursor(0);
    }
}

string CInput::getJoyName(unsigned element) {
    return sinput->getJoyName(element);
}

string CInput::getKeyName(unsigned element) {
    return keyname[element];
}

void CInput::poll(i16* table) {
    memset(table, 0, InputMap::ElementCounter * sizeof(i16));

    pollKeyboard(table);
    table += InputMap::Keyboard::elements;
    pollMouse(table);

    table += InputMap::Mouse::elements;
    sinput->pollJoypad(table);
}

void CInput::pollMouse(i16* table) {
   /* HIPoint point;
    HICoordinateSpace space = 2;
    HIGetMousePosition(space, NULL, &point);
    table[InputMap::Mouse::x] = point.x - relativex;
    table[InputMap::Mouse::y] = point.y - relativey;
    relativex = point.x;
    relativey = point.y;
*/
    int32_t delta_x;
    int32_t delta_y;
    CGGetLastMouseDelta(&delta_x, &delta_y);
    table[InputMap::Mouse::x] = delta_x;
    table[InputMap::Mouse::y] = delta_y;

    unsigned buttonState = GetCurrentEventButtonState();
    for(unsigned i = 0; i < InputMap::Mouse::buttons; i++) {
        table[InputMap::Mouse::button + i] = buttonState & (1 << i);
    }
}

void CInput::pollKeyboard(i16* table) {
    KeyMap keys;
    GetKeys(keys);
    u8* keymap = (u8*)keys;

    #define map(id, name) table[name] = (bool)(keymap[id >> 3] & (1 << (id & 7)))

    map(0x7a, F1);
    map(0x78, F2);
    map(0x63, F3);
    map(0x76, F4);
    map(0x60, F5);
    map(0x61, F6);
    map(0x62, F7);
    map(0x64, F8);
    map(0x65, F9);
    map(0x6d, F10);
    map(0x67, F11);
    //map(0x??, F12);

    map(0x71, Pause);
    map(0x32, Tilde);

    map(0x12, Num1);
    map(0x13, Num2);
    map(0x14, Num3);
    map(0x15, Num4);
    map(0x17, Num5);
    map(0x16, Num6);
    map(0x1a, Num7);
    map(0x1c, Num8);
    map(0x19, Num9);
    map(0x1d, Num0);

    map(0x1b, Dash);
    map(0x18, Equal);
    map(0x33, Backspace);

    map(0x72, Insert);
    map(0x75, Delete);
    map(0x73, Home);
    map(0x77, End);
    map(0x74, PageUp);
    map(0x79, PageDown);

    map(0x00, A);
    map(0x0b, B);
    map(0x08, C);
    map(0x02, D);
    map(0x0e, E);
    map(0x03, F);
    map(0x05, G);
    map(0x04, H);
    map(0x22, I);
    map(0x26, J);
    map(0x28, K);
    map(0x25, L);
    map(0x2e, M);
    map(0x2d, N);
    map(0x1f, O);
    map(0x23, P);
    map(0x0c, Q);
    map(0x0f, R);
    map(0x01, S);
    map(0x11, T);
    map(0x20, U);
    map(0x09, V);
    map(0x0d, W);
    map(0x07, X);
    map(0x10, Y);
    map(0x06, Z);

    map(0x21, LeftBracket);
    map(0x1e, RightBracket);
    map(0x2a, Backslash);
    map(0x29, Semicolon);
    map(0x27, Apostrophe);
    map(0x2b, Comma);
    map(0x2f, Period);
    map(0x2c, Slash);

    map(0x53, Keypad1);
    map(0x54, Keypad2);
    map(0x55, Keypad3);
    map(0x56, Keypad4);
    map(0x57, Keypad5);
    map(0x58, Keypad6);
    map(0x59, Keypad7);
    map(0x5b, Keypad8);
    map(0x5c, Keypad9);
    map(0x52, Keypad0);

    //map(0x??, Separator);
    map(0x4c, Enter);
    map(0x45, Add);
    map(0x4e, Subtract);
    map(0x43, Multiply);
    map(0x4b, Divide);

    map(0x7e, Up);
    map(0x7d, Down);
    map(0x7b, Left);
    map(0x7c, Right);

    map(0x30, Tab);
    map(0x24, Return);
    map(0x31, Spacebar);
    //map(0x??, Menu);

    map(0x38, LeftShift);
    map(0x3b, LeftControl);
    map(0x3a, LeftAlt);
    #undef map
}

void CInput::init(unsigned long long _handle) {
    handle = _handle;
    sinput->init();

    relativex = 0;
    relativey = 0;
}

void CInput::term() {
}

string CInput::keyname[] = {"",
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
