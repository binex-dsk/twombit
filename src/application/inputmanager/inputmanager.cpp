
#include "inputmanager.h"
#include "../emulator.h"
#include <helper.h>
#include <sstream>

Inputmanager* Inputmanager::instance = 0;

Inputmanager::Device* Inputmanager::getDevice(Inputmanager::Port port,Inputmanager::DeviceID deviceId) {
    for(unsigned i=0; i < vectorPos; i++) {
        if (deviceList[i]->id == deviceId && deviceList[i]->port == port) {
            return deviceList[i];
        }
    }
    return 0;
}

Inputmanager::InputObject* Inputmanager::Device::getInputObject(Inputmanager::IObjectID iobjectId) {
    for(unsigned i=0; i < vectorPos; i++) {
        if (objectList[i]->id == iobjectId) {
            return objectList[i];
        }
    }
    return 0;
}

bool Inputmanager::areThereMouseInputObjects(Inputmanager::Port port, Inputmanager::DeviceID deviceId) {
    Device* device;
    if ( ( device = getDevice(port, deviceId) ) == 0 ) return 0;
    unsigned pos = InputMap::Keyboard::elements + InputMap::Mouse::x;
    for(unsigned i=0; i < device->countInputObjects(); i++) {
        if ((unsigned(*device->objectList[i]->setting) == pos)
        || (unsigned(*device->objectList[i]->setting) == (pos+1) )
        || (unsigned(*device->objectList[i]->setting) == (pos+2) )) {
                return true;
        }
    }
    return false;
}

i16 Inputmanager::getState(Inputmanager::Port port, Inputmanager::DeviceID deviceId, Inputmanager::IObjectID iobjectId) {
    Device* device;
    if ( ( device = getDevice(port, deviceId) ) == 0 ) return 0;
    InputObject* iobject;
    if ( ( iobject = device->getInputObject(iobjectId) ) == 0 ) return 0;
    return iobject->getState();
}

void Inputmanager::Device::unmap() {
    for(unsigned i=0; i<vectorPos; i++) {
        *(objectList[i] -> setting) = 0;
    }
}

string Inputmanager::InputObject::getEntry() {
    string key_name;

    if ((type == InputObject::Button && !InputElement::isButton(*setting)) ||
        (type == InputObject::Axis && !InputElement::isAxis(*setting)) ||
        (*setting == 0) )
    {
        *setting = 0;
        return name + "|<none>|<none>";
    }

    if (*setting < InputMap::Keyboard::elements) { //Keyboard
        return name + "|Keyboard|" + Emulator::getInstance()->getKeyName(*setting);

    } else if (*setting < (InputMap::Mouse::elements + InputMap::Keyboard::elements)) { //Mouse
        u16 index = *setting - InputMap::Keyboard::elements;

        switch (index) {
            case InputMap::Mouse::x: key_name = "X-Axis"; break;
            case InputMap::Mouse::y: key_name = "Y-Axis"; break;
            case InputMap::Mouse::z: key_name = "Z-Axis"; break;
            default:
                stringstream st;
                st << (index - 2);
                key_name = "Button: " + st.str();
                break;
        }
        return name + "|Mouse|" + key_name;

    } else if (*setting < InputMap::ElementCounter) { //Joypad
        u16 index = (*setting - (InputMap::Mouse::elements + InputMap::Keyboard::elements)) % InputMap::Joypad::elements;
        u8 joy = u8(double(*setting - (InputMap::Mouse::elements + InputMap::Keyboard::elements)) / double(InputMap::Joypad::elements));

        switch (index) {
            case InputMap::Joypad::hat_up:			key_name = "hat0: Up"; break;
            case InputMap::Joypad::hat_up+4:	    key_name = "hat1: Up"; break;
            case InputMap::Joypad::hat_up+8:	    key_name = "hat2: Up"; break;
            case InputMap::Joypad::hat_up+12:	    key_name = "hat3: Up"; break;

            case InputMap::Joypad::hat_right:	    key_name = "hat0: Right"; break;
            case InputMap::Joypad::hat_right+4:     key_name = "hat1: Right"; break;
            case InputMap::Joypad::hat_right+8:     key_name = "hat2: Right"; break;
            case InputMap::Joypad::hat_right+12:	key_name = "hat3: Right"; break;

            case InputMap::Joypad::hat_down:		key_name = "hat0: Down"; break;
            case InputMap::Joypad::hat_down+4:		key_name = "hat1: Down"; break;
            case InputMap::Joypad::hat_down+8:		key_name = "hat2: Down"; break;
            case InputMap::Joypad::hat_down+12:     key_name = "hat3: Down"; break;

            case InputMap::Joypad::hat_left:		key_name = "hat0: Left"; break;
            case InputMap::Joypad::hat_left+4:		key_name = "hat1: Left"; break;
            case InputMap::Joypad::hat_left+8:		key_name = "hat2: Left"; break;
            case InputMap::Joypad::hat_left+12:     key_name = "hat3: Left"; break;

            case InputMap::Joypad::axis:			key_name = "X-Axis"; break;
            case InputMap::Joypad::axis + 1:		key_name = "Y-Axis"; break;
            case InputMap::Joypad::axis + 2:	    key_name = "Z-Axis"; break;
            case InputMap::Joypad::axis + 3:		key_name = "X-Axis rot"; break;
            case InputMap::Joypad::axis + 4:	    key_name = "Y-Axis rot"; break;
            case InputMap::Joypad::axis + 5:	    key_name = "Z-Axis rot"; break;

            default:
                stringstream st;
                st << (index - 21);
                key_name = "Button: " + st.str();
                break;
        }
        return name + "|" + Emulator::getInstance()->getJoyName(joy) + "|" + key_name;
    }
    return name + "|<none>|<none>";
}

Inputmanager::InputElement::Type Inputmanager::InputElement::getElementType(unsigned element) {
    Type type;

    if (element < InputMap::Keyboard::elements) {
        type = KeyboardButton;
    } else if (element < (InputMap::Mouse::elements + InputMap::Keyboard::elements)) {
        u16 index = element - InputMap::Keyboard::elements;

        switch (index) {
            case InputMap::Mouse::x:
            case InputMap::Mouse::y:
            case InputMap::Mouse::z:
                type = MouseAxis;
                break;
            default:
                type = MouseButton;
                break;
        }
    } else if (element < InputMap::ElementCounter) {

        u16 index = (element - (InputMap::Mouse::elements + InputMap::Keyboard::elements)) % InputMap::Joypad::elements;

        switch (index) {
            case InputMap::Joypad::hat_up:		//+4 +8 +12
            case InputMap::Joypad::hat_right:	  //+4 +8 +12
            case InputMap::Joypad::hat_down:	  //+4 +8 +12
            case InputMap::Joypad::hat_left:	  //+4 +8 +12
            default:
                type = JoypadButton;
                break;

            case InputMap::Joypad::axis:
            case InputMap::Joypad::axis + 1:
            case InputMap::Joypad::axis + 2:
            case InputMap::Joypad::axis + 3:
            case InputMap::Joypad::axis + 4:
            case InputMap::Joypad::axis + 5:
                type = JoypadAxis;
                break;
        }
    }
    return type;
}

void Inputmanager::captureReady(InputObject* iobject) {
    if (waiting) {
        return;
    }
    //pollDevices();
    fetchInput();
    waiting = true;
    captureObject = iobject;
}

void Inputmanager::fetchInput() {
    activeState ^= 1;
    Emulator::getInstance()->fetchInputTable( inputTable[activeState] );
}
bool Inputmanager::pollDevices() {
    if (!waiting) return false;
    bool last = activeState;
    fetchInput();

    //remap
    for(unsigned i=0; i < InputMap::ElementCounter; i++) {

        if (inputTable[activeState][i] == inputTable[last][i]) continue;
        if (! (InputElement::isAxis(i) || inputTable[activeState][i]) ) continue;

        InputObject::Type type = captureObject -> type;

        if ( type == InputObject::Button && !InputElement::isButton(i) ) continue;

        if (type == InputObject::Axis) {
            if (InputElement::isButton(i)) continue;

            i16 element_state = inputTable[activeState][i];

            if (InputElement::getElementType(i) == InputElement::MouseAxis && abs(element_state) < 8) {
                continue;
            }

            if ( (InputElement::getElementType(i) == InputElement::JoypadAxis) &&
                (element_state > -28672 && element_state < +28672)) {
                continue;
            }
        }

        *(captureObject -> setting) = i;
        waiting = false;
        flush();
        return true;
    }
    return false;
}

void Inputmanager::Device::updateState(i16* table) {
    for(unsigned i = 0; i < vectorPos; i++) {
        if(InputElement::getElementType(*objectList[i]->setting) == InputElement::MouseAxis && !Emulator::getInstance()->isInputAcquired()) {
            objectList[i]->state = 0; //mouse must be acquired (locked to window) to move axes
        } else if(InputElement::getElementType(*(objectList[i]->setting)) == InputElement::JoypadAxis) {
            //joypad axis range = -32768 to +32767, scale to -8 to +7 to roughly match mouse delta
            objectList[i]->state = table[*objectList[i]->setting] / 4096;
        } else {
            objectList[i]->state = table[*objectList[i]->setting];
        }
    }
}


