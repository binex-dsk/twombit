
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <dataTypes.h>
#include "inputmap.h"
#include "../settings.h"
#include <string>
using namespace std;


class Inputmanager
{
    static Inputmanager* instance;

public:
    static Inputmanager* getInstance() {
        return instance;
    }
    enum DeviceID {
        IdNone = 0,
        IdJoypad = 1,
        IdLightgun = 2,
        IdPaddle = 3,
        IdSportsPad = 4,
        IdTerebi = 5,
        IdMisc = 6
    };

    enum IObjectID {
        Up		=	0, Down =	1,
        Left	=	2, Right=	3,
        A		=	4, B	=	5,
        Start   =   6,
        X		=	7, Y	=	8,
        Trigger	=	9, Pause=	10
    };

    enum Port { Port1 = 0, Port2 = 1, PortNone = 2 };

	struct InputElement {
		enum Type { KeyboardButton, MouseAxis, MouseButton, JoypadAxis, JoypadButton };

		static Type getElementType(unsigned element);

		static bool isButton(unsigned element) {
			Type id = getElementType(element);
			return (id == KeyboardButton || id == MouseButton || id == JoypadButton);
		}

		static bool isAxis(unsigned element) {
			Type id = getElementType(element);
			return (id == MouseAxis || id == JoypadAxis);
		}
	};

	struct InputObject {
		IObjectID id;
        Inputmanager* inputmanager;
		enum Type { Button, Axis } type;
        string name;
        Settings* setting;
        i16 state;
        string getEntry();
        i16 getState() {
            return state;
        }
        bool isButton() {
            return type == Button;
        }
        string getName() {
            return name;
        }

        InputObject(Type type, string name, IObjectID id, Settings* setting, Inputmanager* inputmanager)
        : type(type), name(name), id(id), setting(setting), inputmanager(inputmanager)
        {
            if (*setting >= InputMap::ElementCounter) *setting = 0;
        }
	};

	struct Device {
		Inputmanager* inputmanager;
		DeviceID id;
        Port port;
        string name;
		unsigned vectorPos;
		std::vector<InputObject*> objectList;

		unsigned countInputObjects() const { return vectorPos; }
		void unmap();

		void attach(InputObject& iobject) {
			objectList[vectorPos++] = &iobject;
		}

        std::vector<InputObject*> getObjectList() {
            return objectList;
        }

		void updateState(i16* table);

        string getDeviceName() { return name; }
        Port getDevicePort() { return port; }
        unsigned getDeviceId() { return (unsigned)id; }
        InputObject* getInputObject(IObjectID iobjectId);

        Device(DeviceID id, Port port, string name, Inputmanager* inputmanager)
		: objectList(10), name(name), id(id), port(port), inputmanager(inputmanager) { vectorPos = 0; }
	};

	struct Joypad : Device {
        InputObject up, down, left, right, a, b, start;

        Joypad(DeviceID id, Port port, string name, Inputmanager* inputmanager
            ,Settings* _up, Settings* _down, Settings* _left, Settings* _right
            ,Settings* _a, Settings* _b, Settings* _start
        )
		:
		Device(id, port, name, inputmanager),

        up		(InputObject::Button, "Up", Up, _up, inputmanager),
        down	(InputObject::Button, "Down", Down, _down, inputmanager),
        left	(InputObject::Button, "Left", Left, _left, inputmanager),
        right	(InputObject::Button, "Right", Right, _right, inputmanager),
        a		(InputObject::Button, "A", A, _a, inputmanager),
        b		(InputObject::Button, "B", B, _b, inputmanager),
        start	(InputObject::Button, "Start", Start, _start, inputmanager)
		{
            attach(up); attach(down); attach(left); attach(right); attach(a); attach(b); attach(start);
		}
	};

	struct Lightgun : Device {
		InputObject x, y, trigger;

        Lightgun(DeviceID id, Port port, string name, Inputmanager* inputmanager
            ,Settings* _x, Settings* _y, Settings* _trigger
        )
		:
		Device(id, port, name, inputmanager),
        x      (InputObject::Axis,   "X-Axis", X, _x, inputmanager),
        y      (InputObject::Axis,   "Y-Axis", Y, _y, inputmanager),
        trigger(InputObject::Button, "Trigger", Trigger, _trigger, inputmanager)
		{
			attach(x); attach(y); attach(trigger);
		}
	};

    struct Terebi : Device {
        InputObject x, y, trigger;

        Terebi(DeviceID id, Port port, string name, Inputmanager* inputmanager
            ,Settings* _x, Settings* _y, Settings* _trigger
        )
        :
        Device(id, port, name, inputmanager),
        x      (InputObject::Axis,   "X-Axis", X, _x, inputmanager),
        y      (InputObject::Axis,   "Y-Axis", Y, _y, inputmanager),
        trigger(InputObject::Button, "Trigger", Trigger, _trigger, inputmanager)
        {
            attach(x); attach(y); attach(trigger);
        }
    };

    struct Paddle : Device {
        InputObject x, a;

        Paddle(DeviceID id, Port port, string name, Inputmanager* inputmanager
        ,Settings* _x, Settings* _a
        )
        :
        Device(id, port, name, inputmanager),
        x   (InputObject::Axis,   "X-Axis", X, _x, inputmanager),
        a   (InputObject::Button, "A", A, _a, inputmanager)
        {
            attach(x); attach(a);
        }
    };

    struct SportsPad : Device {
        InputObject x, y, a, b;

        SportsPad(DeviceID id, Port port, string name, Inputmanager* inputmanager
        ,Settings* _x, Settings* _y, Settings* _a, Settings* _b
        )
        :
        Device(id, port, name, inputmanager),
        x   (InputObject::Axis,   "X-Axis", X, _x, inputmanager),
        y   (InputObject::Axis,   "Y-Axis", Y, _y, inputmanager),
        a   (InputObject::Button, "A", A, _a, inputmanager),
        b   (InputObject::Button, "B", B, _b, inputmanager)
        {
            attach(x);  attach(y); attach(a); attach(b);
        }
    };

	struct PauseButton : Device {
		InputObject pause;

        PauseButton(DeviceID id, Port port, string name, Inputmanager* inputmanager, Settings* _pause)
		:
		Device(id, port, name, inputmanager),
        pause(InputObject::Button, "Pause", Pause, _pause, inputmanager)
		{
			attach(pause);
		}
	};

	Joypad joypad1;
	Joypad joypad2;
	Lightgun lightgun1;
    Terebi terebi;
	Lightgun lightgun2;
    Paddle paddle1;
    Paddle paddle2;
    SportsPad sportspad1;
    SportsPad sportspad2;
	PauseButton pause;

	std::vector<Device*> deviceList;
	unsigned vectorPos;

	void attach(Device& device) {
		deviceList[vectorPos++] = &device;
	}

    std::vector<Device*> getDeviceList() {
        return deviceList;
    }

    void captureReady(InputObject* captureObject);
    Device* getDevice(Port port, DeviceID deviceId);
	unsigned getDeviceCount() const { return vectorPos; }
    i16 getState(Port port, DeviceID deviceId, IObjectID iobjectId);
    bool areThereMouseInputObjects(Port port, DeviceID deviceId);

    i16 inputTable[2][InputMap::ElementCounter];

	bool isCapturing() { return waiting; }
	void endCapturing() { waiting = false; }

	bool activeState;
	bool waiting;
	bool pollDevices();
    void fetchInput();
	InputObject* captureObject;

	void flush() {
        for(unsigned i = 0; i < InputMap::ElementCounter; i++) {
			inputTable[0][i] = inputTable[1][i] = 0;
		}
	}

	void update() {
		for(unsigned i=0; i < vectorPos; i++) {
			deviceList[i]->updateState( inputTable[activeState] );
		}
	}

    Inputmanager()
	:
    joypad1( IdJoypad, Port1, "Joypad", this
            ,Settings::findSetting("sms_joypad_1_up")
            ,Settings::findSetting("sms_joypad_1_down")
            ,Settings::findSetting("sms_joypad_1_left")
            ,Settings::findSetting("sms_joypad_1_right")
            ,Settings::findSetting("sms_joypad_1_a")
            ,Settings::findSetting("sms_joypad_1_b")
            ,Settings::findSetting("sms_joypad_1_start")
    ),
    joypad2( IdJoypad, Port2, "Joypad", this
             ,Settings::findSetting("sms_joypad_2_up")
             ,Settings::findSetting("sms_joypad_2_down")
             ,Settings::findSetting("sms_joypad_2_left")
             ,Settings::findSetting("sms_joypad_2_right")
             ,Settings::findSetting("sms_joypad_2_a")
             ,Settings::findSetting("sms_joypad_2_b")
             ,Settings::findSetting("sms_joypad_2_start")
    ),
    lightgun1( IdLightgun, Port1, "Lightgun", this
            ,Settings::findSetting("sms_lightgun_1_x")
            ,Settings::findSetting("sms_lightgun_1_y")
            ,Settings::findSetting("sms_lightgun_1_trigger")
    ),
    lightgun2( IdLightgun, Port2, "Lightgun", this
            ,Settings::findSetting("sms_lightgun_2_x")
            ,Settings::findSetting("sms_lightgun_2_y")
            ,Settings::findSetting("sms_lightgun_2_trigger")
    ),
    terebi( IdTerebi, Port1, "Terebi Oekaki", this
            ,Settings::findSetting("sms_terebi_1_x")
            ,Settings::findSetting("sms_terebi_1_y")
            ,Settings::findSetting("sms_terebi_1_trigger")
    ),
    paddle1( IdPaddle, Port1, "Paddle", this
            ,Settings::findSetting("sms_paddle_1_x")
            ,Settings::findSetting("sms_paddle_1_a")
    ),

    paddle2( IdPaddle, Port2, "Paddle", this
            ,Settings::findSetting("sms_paddle_2_x")
            ,Settings::findSetting("sms_paddle_2_a")
    ),
    sportspad1( IdSportsPad, Port1, "SportsPad", this
            ,Settings::findSetting("sms_sports_pad_1_x")
            ,Settings::findSetting("sms_sports_pad_1_y")
            ,Settings::findSetting("sms_sports_pad_1_a")
            ,Settings::findSetting("sms_sports_pad_1_b")
    ),
    sportspad2( IdSportsPad, Port2, "SportsPad", this
            ,Settings::findSetting("sms_sports_pad_2_x")
            ,Settings::findSetting("sms_sports_pad_2_y")
            ,Settings::findSetting("sms_sports_pad_2_a")
            ,Settings::findSetting("sms_sports_pad_2_b")
    ),
    pause( IdMisc, PortNone, "Pause", this
            ,Settings::findSetting("sms_pause_pause")
    ),
	deviceList(10)
	{
        instance = this;
		vectorPos = 0;
		activeState = 0;
		waiting = false;
		flush();
		captureObject = 0;
        attach(joypad1); attach(joypad2);
        attach(lightgun1); attach(lightgun2);
        attach(paddle1); attach(paddle2);
        attach(sportspad1); attach(sportspad2);
        attach(terebi);
        attach(pause);
	}
};

#endif
