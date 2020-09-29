#include "openal.h"

ALCdevice* Openal::device = 0;
ALCcontext* Openal::context = 0;

Openal::Openal(unsigned _id)
{
    id = _id;
    if (id == 0) {
        create();
    }
    alGenSources(1, &source);
    format = AL_FORMAT_STEREO16;
    queue_length = 0;

    buffer.data = 0;
    buffer.length = 0;
    buffer.size = 0;

    settings.frequency = 44100;
    settings.latency = 40;
    settings.audio_active = true;
    settings.volume = 1.0;
    settings.speed = 100;
}

Openal::~Openal() {
    freeBuffer();
    if( alIsSource(source) == AL_TRUE ) {
        alDeleteSources(1, &source);
    }
    term();
}

void Openal::setSample(unsigned sample) {
    if (!settings.audio_active) sample = 0;
    buffer.data[buffer.length++] = sample;
    if(buffer.length < buffer.size) return;

    ALuint albuffer = 0;
    int processed = 0;
    while(true) {
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        while(processed--) {
            alSourceUnqueueBuffers(source, 1, &albuffer);
            alDeleteBuffers(1, &albuffer);
            queue_length--;
        }
        if(queue_length < 3) break;
    }

    alGenBuffers(1, &albuffer);
    alBufferData(albuffer, format, buffer.data, buffer.size * 4, settings.frequency);
    alSourceQueueBuffers(source, 1, &albuffer);
    queue_length++;

    play();
    buffer.length = 0;
}

void Openal::stop() {
    ALint playing;
    alGetSourcei(source, AL_SOURCE_STATE, &playing);
    if(playing == AL_PLAYING) alSourceStop(source);
}

void Openal::play() {
    ALint playing;
    alGetSourcei(source, AL_SOURCE_STATE, &playing);
    if(playing != AL_PLAYING) alSourcePlay(source);
}

void Openal::clear() {
    buffer.length = 0;
    if(buffer.data) {
        memset(buffer.data, 0, buffer.size * sizeof(unsigned));
    }

    if( alIsSource(source) == AL_TRUE ) {
        int playing = 0;
        alGetSourcei(source, AL_SOURCE_STATE, &playing);
        if( playing == AL_PLAYING ) {
            alSourceStop(source);
        }
        int queued = 0;
        alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
        while(queued--) {
            ALuint albuffer = 0;
            alSourceUnqueueBuffers(source, 1, &albuffer);
            alDeleteBuffers(1, &albuffer);
        }

    }
    queue_length = 0;
}

void Openal::adjustVolume(unsigned _volume) {
    settings.volume = _volume;
    alSourcef(source, AL_GAIN, ALfloat(settings.volume) / 100);
}

void Openal::generateBuffer() {
    freeBuffer();
    buffer.size = settings.frequency * settings.latency / 1000.0 + 0.5;
    buffer.data = new unsigned[buffer.size];
    queue_length = 0;
}

void Openal::init(unsigned long long handle, unsigned _speed, signed _frequencyCorrection) {
    settings.speed = _speed;
    settings.frequency = 44100 + _frequencyCorrection;
    settings.frequency = (double(settings.frequency) * double(settings.speed)) / 100.0;
    clear();
    generateBuffer();
}

void Openal::create() {
    term();

    if(!(device = alcOpenDevice(NULL))) {
        term();
        throw "openal_error";
    }

    if(!(context = alcCreateContext(device, NULL))) {
        term();
        throw "openal_error";
    }

    alcMakeContextCurrent(context);

    alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
    alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
    ALfloat listener_orientation[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    alListenerfv(AL_ORIENTATION, listener_orientation);
}

void Openal::term() {
    if(context) {
        alcMakeContextCurrent(0);
        alcDestroyContext(context);
        context = 0;
    }

    if(device) {
        alcCloseDevice(device);
        device = 0;
    }
}

void Openal::freeBuffer() {
    if(buffer.data) {
        delete[] buffer.data;
        buffer.data = 0;
    }
}
