#ifndef APP_H
#define APP_H

#include <QString>
#include <QApplication>
#include "lang.h"
#include "config.h"
#include "../gui/mgui.h"
#include "../gui/cgui.h"
#include "emulator.h"

class Exception;

class App : public QApplication {
    Q_OBJECT

private:
    static QString app_dir;
    QTimer timer;
    Lang lang;
    Config config;
    MGui mgui;
    CGui cgui;
    Emulator emulator;

    void setStyle();
    void loadConfig();
    void setLanguage();
    void initSystems();
    void loadFromCommandLine();
    virtual bool notify(QObject * receiver, QEvent * event);
#ifdef __APPLE__
    virtual bool event(QEvent * event);
#endif

public:
    App(int& argc, char** argv);
    void Create();
    int Run();
    static QString getDir() {
        return app_dir;
    }

public slots:
    void Loop();
};

#endif // APP_H
