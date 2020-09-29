#ifndef DSOUND_H
#define DSOUND_H

#include <dsound.h>
#include <dataTypes.h>

#include "../drvaudio.h"

#define DSVOLUME_TO_DB(volume)  (volume == 0 ? DSBVOLUME_MIN : ((DWORD)(-30*(100 - volume))))

class DSound : public DrvAudio {
public:
    void stop();
    void play();
    void create(HWND);
    void term();
    void clear();
    void setSample(u32 sample);
    DSound(unsigned _id);
    ~DSound() { term(); }

    struct {
        unsigned speed;
        unsigned frequency;
        unsigned latency;
        unsigned volume;
        bool audio_active;
    } settings;

    void activateAudio(bool state) {
        settings.audio_active = state;
    }
    void init(HWND hwnd, unsigned _speed, signed _frequencyCorrection);
    void adjustVolume(unsigned _volume);

private:
    LPDIRECTSOUND ds;
    DSBUFFERDESC dsbd;
    WAVEFORMATEX wfx;
    LPDIRECTSOUNDBUFFER dsb;
    struct {
        unsigned readring;
        unsigned writering;
        int distance;
        u8 rings;
        unsigned buffer_pos;
        unsigned buffer_size;
        u32* buffer;
        unsigned latency;
        unsigned frequency;
    } device;
};

#endif // DSOUND_H
