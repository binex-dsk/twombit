#ifndef OPENAL_H
#define OPENAL_H

#ifdef __linux__
    #include <AL/al.h>
    #include <AL/alc.h>
#elif __APPLE__
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#endif

#include <string.h>

#include "../drvaudio.h"


class Openal : public DrvAudio
{
    unsigned id;
    ALuint source;
    ALenum format;
    unsigned queue_length;

    static ALCcontext* context;
    static ALCdevice* device;

    struct {
        unsigned speed;
        unsigned frequency;
        unsigned latency;
        unsigned volume;
        bool audio_active;
    } settings;

    struct {
        unsigned* data;
        unsigned length;
        unsigned size;
    } buffer;

    void freeBuffer();
    void generateBuffer();
    static void create();
    static void term();

public:
    Openal(unsigned _id);
    ~Openal();
    void activateAudio(bool state) {
        settings.audio_active = state;
    }
    void init(unsigned long long handle, unsigned _speed, signed _frequencyCorrection);
    void stop();
    void play();
    void clear();
    void setSample(unsigned sample);
    void adjustVolume(unsigned _volume);
};

#endif // OPENAL_H
