#include "sinput.h"

SInput::SInput()
{
    for(unsigned i = 0; i < InputMap::Joypad::count; i++) {
        gamepad[i] = 0;
    }
}

SInput::~SInput() {
    term();
}

void SInput::pollJoypad(i16* table) {
    SDL_JoystickUpdate();

    for(unsigned i = 0; i < InputMap::Joypad::count; i++) {
        if(!gamepad[i]) continue;
        i16* addr = table + i * InputMap::Joypad::elements;

        //POV hats
        unsigned hats = _min((unsigned)InputMap::Joypad::hats, SDL_JoystickNumHats(gamepad[i]));
        for(unsigned hat = 0; hat < hats; hat++) {
            u8 state = SDL_JoystickGetHat(gamepad[i], hat);
            unsigned x = hat * InputMap::Joypad::hat_directions;

            if(state & SDL_HAT_UP   ) *(addr + InputMap::Joypad::hat_up + x) = true;
            if(state & SDL_HAT_RIGHT) *(addr + InputMap::Joypad::hat_right + x) = true;
            if(state & SDL_HAT_DOWN ) *(addr + InputMap::Joypad::hat_down + x) = true;
            if(state & SDL_HAT_LEFT ) *(addr + InputMap::Joypad::hat_left + x) = true;
        }

        //axes
        unsigned axes = _min((unsigned)InputMap::Joypad::axes, SDL_JoystickNumAxes(gamepad[i]));
        for(unsigned axis = 0; axis < axes; axis++) {
            *(addr + InputMap::Joypad::axis + axis) = (i16)SDL_JoystickGetAxis(gamepad[i], axis);
        }

        //buttons
        for(unsigned b = 0; b < InputMap::Joypad::buttons; b++) {
            *(addr + InputMap::Joypad::button + b) = (bool)SDL_JoystickGetButton(gamepad[i], b);
        }
    }
}

string SInput::getJoyName(unsigned element) {
    if (SDL_NumJoysticks() <= element)
        return "error";
    return SDL_JoystickName(element);
}

void SInput::init() {
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    unsigned joyCount = _min((unsigned)InputMap::Joypad::count, SDL_NumJoysticks());
    for(unsigned i = 0; i < joyCount; i++) {
        gamepad[i] = SDL_JoystickOpen(i);
    }
}

void SInput::term() {
    unsigned joyCount = _min((unsigned)InputMap::Joypad::count, SDL_NumJoysticks());
    for(unsigned i = 0; i < joyCount; i++) {
        if(gamepad[i]) SDL_JoystickClose(gamepad[i]);
        gamepad[i] = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}
