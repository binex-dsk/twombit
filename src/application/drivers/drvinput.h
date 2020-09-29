#ifndef DRVINPUT_H
#define DRVINPUT_H

#include <string>
using namespace std;

class DrvInput {
public:
    virtual void poll(i16* table) = 0;
    virtual string getKeyName(unsigned element) = 0;
    virtual string getJoyName(unsigned element) = 0;
    template <typename T> void init(T handle) {}
    virtual void m_acquire() = 0;
    virtual void m_unacquire() = 0;
    virtual bool m_isAcquired() = 0;
};

#endif // DRVINPUT_H
