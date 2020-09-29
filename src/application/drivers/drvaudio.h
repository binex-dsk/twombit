#ifndef DRVAUDIO_H
#define DRVAUDIO_H

class DrvAudio {
public:
    virtual void stop() = 0;
    virtual void play() = 0;
    virtual void clear() = 0;
    virtual void setSample(unsigned sample) = 0;
    virtual void activateAudio(bool state) = 0;
    virtual void adjustVolume(unsigned _volume) = 0;
};

#endif // DRVAUDIO_H
