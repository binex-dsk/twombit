#include "dinput.h"

#define dx_release(__m) if(__m) { __m->Release(); __m = 0; }

DInput::DInput()
{
	din = 0;
	din_key = din_mouse = 0;
	din_joy_count = 0;
	m_acquired = false;

    for (unsigned i=0; i < InputMap::Joypad::count; i++) {
		din_joy[i] = 0;
	}
}

void DInput::init(HWND handle)
{
    m_acquired = false;
    hwnd = handle;
	term();

    if (FAILED(DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&din, 0))) {
        throw "din_error";
	}

	din->CreateDevice(GUID_SysKeyboard, &din_key, 0);

	din_key->SetDataFormat(&c_dfDIKeyboard);
    din_key->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	din_key->Acquire();

	din->CreateDevice(GUID_SysMouse, &din_mouse, 0);

	din_mouse->SetDataFormat(&c_dfDIMouse2);
    din_mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	din_mouse->Acquire();

	din->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoypadsCallback, this, DIEDFL_ATTACHEDONLY);
}

BOOL CALLBACK DInput::EnumJoypadsCallback(const DIDEVICEINSTANCE *instance, void *pRef)
{
	DInput* ptr = (DInput*)pRef;
	return ptr->enum_joypads(instance);
}

bool DInput::enum_joypads(const DIDEVICEINSTANCE* instance)
{
	HRESULT hr = din->CreateDevice(instance->guidInstance, &din_joy[din_joy_count], 0);

	if(FAILED(hr)) return DIENUM_CONTINUE;

    device_name[din_joy_count] = (string) instance->tszProductName;

	din_joy[din_joy_count]->SetDataFormat(&c_dfDIJoystick2);
	din_joy[din_joy_count]->SetCooperativeLevel(0, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

    if(++din_joy_count >= InputMap::Joypad::count) return DIENUM_STOP;

	return DIENUM_CONTINUE;
}

void DInput::m_acquire()
{
    if(!din_mouse) return;
    if(!m_acquired) {
		din_mouse->Unacquire();
        din_mouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		din_mouse->Acquire();
		m_acquired = true;
    }
}

void DInput::m_unacquire()
{
    if(!din_mouse) return;
    if(m_acquired) {
		din_mouse->Unacquire();
        din_mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		din_mouse->Acquire();
		m_acquired = false;
    }
}


void DInput::poll(i16* i_map)
{
    memset(i_map, 0, InputMap::ElementCounter * sizeof(i16));

	if(din_key) {

        u8 keystate[InputMap::Keyboard::elements];

        if(FAILED( din_key -> GetDeviceState(InputMap::Keyboard::elements, (LPVOID)keystate))) {
			din_key -> Acquire();
            if(FAILED( din_key -> GetDeviceState(InputMap::Keyboard::elements, (LPVOID)keystate))) {
				memset(keystate, 0, sizeof(keystate));
			}
		}

		for(unsigned i=0; i < sizeof(keystate); i++) {
			*(i_map + i) = !!(keystate[i] & 0x80);
		}
	}

    i_map += InputMap::Keyboard::elements;

	if (din_mouse) {

		DIMOUSESTATE2 m_state;

		if(FAILED( din_mouse -> GetDeviceState(sizeof(DIMOUSESTATE2), (void*)&m_state))) {
			din_mouse -> Acquire();
			if(FAILED( din_mouse -> GetDeviceState(sizeof(DIMOUSESTATE2), (void*)&m_state))) {
				memset(&m_state, 0, sizeof(DIMOUSESTATE2));
			}
		}

        i_map[InputMap::Mouse::x] = m_state.lX;
        i_map[InputMap::Mouse::y] = m_state.lY;
        i_map[InputMap::Mouse::z] = m_state.lZ / WHEEL_DELTA;

        for(unsigned i = 0; i < InputMap::Mouse::buttons; i++) {
            i_map[InputMap::Mouse::button + i] = (bool)m_state.rgbButtons[i];
		}
	}

    i_map += InputMap::Mouse::elements;

	DIJOYSTATE2 js;

	for(int i = 0; i < din_joy_count; i++) {
		if(!din_joy[i]) continue;

        memset(js.rgbButtons, 0, InputMap::Joypad::buttons);

		if(FAILED( din_joy[i] -> Poll())) {
			din_joy[i]->Acquire();
			if(FAILED( din_joy[i]->Poll())) {
				continue;
			}
		}

		din_joy[i] -> GetDeviceState(sizeof(DIJOYSTATE2), &js);

        i16* addr = i_map + i * InputMap::Joypad::elements;

		unsigned x;

		int resistance_lo = 0x7fff - 28672;
		int resistance_hi = 0x8000 + 28672;

        *(addr + InputMap::Joypad::hat_up) = (js.lY <= resistance_lo);
        *(addr + InputMap::Joypad::hat_right) = (js.lX >= resistance_hi);
        *(addr + InputMap::Joypad::hat_down) = (js.lY >= resistance_hi);
        *(addr + InputMap::Joypad::hat_left) = (js.lX <= resistance_lo);
	 

        for(int n = 0; n < InputMap::Joypad::hats; n++) {
			unsigned pov = js.rgdwPOV[n];
			if(pov < 36000) {
                x = n * InputMap::Joypad::hat_directions;
                if(pov >= 31500 || pov <=  4500) *(addr + InputMap::Joypad::hat_up + x) = true;
                if(pov >=  4500 && pov <= 13500) *(addr + InputMap::Joypad::hat_right + x) = true;
                if(pov >= 13500 && pov <= 22500) *(addr + InputMap::Joypad::hat_down + x) = true;
                if(pov >= 22500 && pov <= 31500) *(addr + InputMap::Joypad::hat_left + x) = true;
			}
		}

        *(addr + InputMap::Joypad::axis + 0) = js.lX - 32768;
        *(addr + InputMap::Joypad::axis + 1) = js.lY - 32768;
        *(addr + InputMap::Joypad::axis + 2) = js.lZ - 32768;
        *(addr + InputMap::Joypad::axis + 3) = js.lRx - 32768;
        *(addr + InputMap::Joypad::axis + 4) = js.lRy - 32768;
        *(addr + InputMap::Joypad::axis + 5) = js.lRz - 32768;

        for(unsigned b = 0; b < InputMap::Joypad::buttons; b++) {
            *(addr + InputMap::Joypad::button + b) = (bool)js.rgbButtons[b];
		}
	}
}

void DInput::term()
{
	dx_release(din);
	dx_release(din_key);
	dx_release(din_mouse);
	
    for (int i=0; i < InputMap::Joypad::count; i++) {
		dx_release(din_joy[i]);
		device_name[i] = "<unknown>";
	}

	din_joy_count = 0;
}

DInput::~DInput()
{
	term();
}

