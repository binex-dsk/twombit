#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include "dialog.h"

class Exception {
   QString const msg;
public:
   Exception(QString msg) : msg(msg) {}

   int handle() const {
        try {
            if (msg.length() != 0) {
                Dialog::Fail( msg );
            } else {
                return EXIT_SUCCESS;
            }
        } catch (...) {
            Dialog::Fail( "Unhandled error! Here goes nothing!" );
        }
        return EXIT_FAILURE;
    }
};

#endif
