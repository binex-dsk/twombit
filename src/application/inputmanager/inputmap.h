#ifndef INPUTMAP_H
#define INPUTMAP_H

namespace InputMap {
    struct Keyboard { enum { elements = 256 }; };

    struct Mouse {
        enum { buttons = 8 };
        enum { x, y, z, button, elements = button + buttons };
    };

    struct Joypad {
        enum { count = 16 };
        enum { hats = 4 };
        enum { axes = 6 };
        enum { buttons = 96 };
        enum { hat_up, hat_right, hat_down, hat_left, hat_directions };

        enum { hat, axis = hat + (hats * hat_directions), button = axis + axes, elements = button + buttons };
    };

    enum { ElementCounter = Keyboard::elements + Mouse::elements + Joypad::count * Joypad::elements };
}

#endif // INPUTMAP_H
