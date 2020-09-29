#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include "file.h"
#include <QString>

class StateHandler
{
    File state;
    static StateHandler* instance;
public:
    unsigned char* load(QString fn, QString ident, unsigned pos);
    bool save(QString fn, QString ident, unsigned pos, unsigned char* buffer, unsigned length);
    QString getPath(QString ident);
    unsigned getSize();

    StateHandler();
    static StateHandler* getInstance() {
        if (instance == 0) {
            instance = new StateHandler();
        }
        return instance;
    }
};

#endif // STATEHANDLER_H
