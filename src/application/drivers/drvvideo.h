#ifndef DRVVIDEO_H
#define DRVVIDEO_H

class DrvVideo {
public:
    virtual bool lock(unsigned*& data, unsigned& pitch, unsigned width, unsigned height) = 0;
    virtual void unlock() = 0;
    virtual void redraw() = 0;
    virtual void set_counter(bool state) = 0;
    virtual void set_counter_pos(unsigned pos) = 0;
};

#endif // DRVVIDEO_H
