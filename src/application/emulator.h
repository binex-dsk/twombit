#ifndef EMULATOR_H
#define EMULATOR_H

#include <QVector>
#include <QString>
#include <QLibrary>
#include "colorInterface/color.h"
#include "../gui/mgui.h"
#include "../gui/cgui.h"

#ifdef _WIN32
    #include "drivers/directx/dinput.h"
    #include "drivers/directx/dsound.h"
    #include "drivers/directx/dvideo.h"
#elif __linux__
    #include "drivers/openal/openal.h"
    #include "drivers/x11/xinput.h"
    #include "drivers/opengl/qtopengl.h"
#elif __APPLE__
    #include "drivers/openal/openal.h"
    #include "drivers/carbon/cinput.h"
    #include "drivers/opengl/qtopengl.h"
#else
    #error "no driver for this platform available"
#endif

#include "inputmanager/inputmanager.h"
#include "resample/resampler.h"
#include "file.h"

class CGui;
class MGui;

class Emulator
{
    CGui& cgui;
    MGui& mgui;
    Color color;
    #ifdef _WIN32
        DVideo drvVideo;
        DVideo drvVideoGG;
        DSound drvAudio;
        DSound drvAudioGG;
        DInput drvInput;
    #elif __linux__
        QtOpenGL drvVideo;
        Openal drvAudio;
        Openal drvAudioGG;
        XInput drvInput;
    #elif __APPLE__
        QtOpenGL drvVideo;
        Openal drvAudio;
        Openal drvAudioGG;
        CInput drvInput;
    #else
        #error "no driver for this platform available"
    #endif
    Inputmanager inputmanager;
    Resampler resampler;

public:
    struct Models {
        unsigned id;
        QString ident;
        QString desc;
        bool inUse;
        QString extension;
    };

    struct Devices {
        unsigned id;
        QString name;
    };

    bool openRom(QString fn, Models _model);
    void unload();
    void power();
    void reset();
    void run();
    bool isCartLoaded() {
        return cartLoaded;
    }
    void updateColorTable();
    void updateSwFilter();
    void updateHwFilter();
    void init();
    void playSound();
    void stopSound();
    void clearAudio();
    void updateFrequency();
    void adjustVolume(unsigned volume);
    void activateSound(bool state);
    void updateVideo(unsigned width, unsigned height, bool fullscreen);
    void clearVideo();
    void updateScanlines();
    void activateFPS(bool state);
    void inputAcquire();
    void inputUnacquire();
    bool isInputAcquired();
    void reinitInput();
    string getKeyName(unsigned element);
    string getJoyName(unsigned joy);
    void fetchInputTable(i16* table);
    void pollInputDevices();

private:
    static QVector<Models> models;
    Models model;
    File file;
    File biosFile;
    static Emulator* instance;
    bool cartLoaded;
    struct {
        unsigned frames_executed, frame_count;
        time_t prev_t, curr_t;
    } fpsCounter[2];
    bool useMouse;

    void loadSG();
    void loadGG();
    void loadSMS();
    void loadBackupRam();
    void saveBackupRam();
    void saveCheat();
    void loadCheat();
    QString getBackupPath(QString fn);
    void initInput();
    void initSound();
    void initVideo();
    void countFrames(unsigned screenPos);
    void setState();

public:
    Emulator(MGui& mgui, CGui& cgui);
    static Emulator* getInstance() {
        return instance;
    }
    static void initModels();
    static Models getModelByIdent(QString ident);
    static QVector<Models> getModels() {
        return models;
    }
    void pollInput();
    static void _pollInput();
    unsigned getDeviceCount();
    QString getDeviceName(unsigned pos);
    QString getDevicePort(unsigned pos);
    void emptyDevice(unsigned pos);
    unsigned getDeviceObjectCount(unsigned pos);
    QString getDeviceObjectEntry(unsigned devPos, unsigned objPos);
    QString getUpdateString(unsigned devPos, unsigned objPos);
    void captureReady(unsigned devPos, unsigned objPos);
    QVector<Devices> getDevicesForPort(unsigned port);
    void updatePort();
    void checkIfMouseIsInUse();
    static signed _readInput(unsigned port, unsigned deviceId, unsigned objectId);
    signed readInput(unsigned port, unsigned deviceId, unsigned objectId);
    static void _audioSample(signed sampleLeft, signed sampleRight, unsigned soundChip);
    void audioSample(signed sampleLeft, signed sampleRight, unsigned soundChip);
    static void _render(const unsigned short* frame, unsigned width, unsigned height, unsigned modeId, bool secondGG);
    void render(const unsigned short* frame, unsigned width, unsigned height, unsigned modeId, bool secondGG);

    bool setCheat(const char* code);
    void resetCheat();
    void saveState();
    void loadState();
    void changeSlot(unsigned pos);
};

#endif //EMULATOR_H
