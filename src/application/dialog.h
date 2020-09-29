#ifndef DIALOG_H
#define DIALOG_H

#include <QString>
#include <QMessageBox>
#include "main.h"

class Dialog {

public:

    static void Fail (QString const text, QString const title = "") {
        QMessageBox::critical( 0, (title.length() != 0 ? title : _PRG) + " Error", text );
    }

    static void Warn (QString const text, QString const title = "") {
        QMessageBox::warning( 0, (title.length() != 0 ? title : _PRG) + " Warning", text );
    }

    static void Inform (QString const text, QString const title = "") {
        QMessageBox::about( 0, title.length() != 0 ? title : _PRG, text );
    }

    static bool Confirm (QString const text, QString const ok, QString const cancel, QString const title = "") {
        int code = QMessageBox::question( 0, title.length() != 0 ? title : _PRG, text, ok, cancel);
        return (code == 0) ? true : false;
    }
};

#endif
