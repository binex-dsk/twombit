#ifndef SINPUT_H
#define SINPUT_H

#include <SDL/SDL.h>
#include "../../inputmanager/inputmap.h"
#include <dataTypes.h>
#include <helper.h>
#include <string>
using namespace std;

class SInput
{
    SDL_Joystick* gamepad[InputMap::Joypad::count];
    void term();

public:
    void init();
    void pollJoypad(i16* table);
    string getJoyName(unsigned element);
    SInput();
    ~SInput();
};

#endif // SINPUT_H
