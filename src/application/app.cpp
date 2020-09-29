#include "app.h"
#include "main.h"
#include "dialog.h"
#include "settings.h"
#include "exception.h"

QString App::app_dir;

App::App(int& argc, char ** argv)
:
QApplication(argc, argv),
lang(),
config(),
mgui(&cgui),
cgui(&mgui),
emulator(mgui, cgui)
{
    setWindowIcon(QIcon(":/tmb.ico"));

    //QSystemTrayIcon trayIcon(QIcon(":/tmb.ico"));
    //trayIcon.show();

    QString dir = applicationDirPath();
#ifdef Q_WS_MAC
    QDir _dir(dir);
    _dir.cdUp();
    _dir.cdUp();
    _dir.cdUp();
    dir = _dir.absolutePath();
#endif
    dir += "/";
    App::app_dir = dir;

    lang.setDir(dir);
    lang.setFile(_LANG_FILE);

    setStyle();
}

void App::setStyle() {
#ifdef __WIN32__
    QString app_style =
    "QMenuBar::item {background-color: rgb(255,255,255);}"
    "QMenuBar::item:selected {border: 1px solid rgb(121,152,241);}"
    "QMenu::item {padding: 2px 25px 2px 20px;background: #FFFFFF;border: 1px solid transparent;}"
    "QMenu::item:selected  {border: 1px solid rgb(121,152,241);background-color: rgb(226,233,254);color: #000000;}"
    "QMenu::separator { width: 1px; height: 1px;}";
    setStyleSheet(app_style);
#else
//    QString app_style =
//    "QMenu {padding: 2px 2px 2px 2px;background: #FFFFFF;border: 1px solid transparent; color: #000000;}"
//    "QMenu::item {padding: 2px 25px 2px 20px;background: #FFFFFF;border: 1px solid transparent; color: #000000; font: normal 12px;}"
//    "QMenu::item:selected  {border: 1px solid rgb(121,152,241); background-color: rgb(226,233,254);color: #000000;}"
//    "QMenu::item:disabled  {color: #6f6f6f;}"
//    "QMenu::separator { height: 1px; padding: 4px 0px 2px 0px; }";
#endif
}

void App::Create() {
    Emulator::initModels();
    loadConfig();
    setLanguage();
    mgui.create();
    cgui.create();
    emulator.init();
    mgui.initSettings();
    cgui.initSettings();
    mgui.openUp();
    loadFromCommandLine();
}

#ifdef __APPLE__
bool App::event(QEvent * event) {
    if (event->type() == QEvent::FileOpen) {
        mgui.load_command(static_cast<QFileOpenEvent*>(event)->file());
    }
    return QApplication::event(event);
}
#endif

void App::loadFromCommandLine() {
#ifndef __APPLE__
    QStringList args = arguments();

    if (args.count() < 2) {
        return;
    }
    mgui.load_command(args[1]);
#endif
}

void App::loadConfig() {
    if ( !config.load(App::app_dir + _CFG_FILE) ) {
        Dialog::Warn( lang.get("cfg_load") );
    }
}

void App::setLanguage() {
    lang.setFile( Settings::get<QString>("language_file") );
    lang.get("ok"); //test language file from ini
    if (lang.isStandardLang()) {
        Settings::set<QString>("language_file", _LANG_FILE);
    }
}

int App::Run() {
    connect(&timer, SIGNAL(timeout()), this, SLOT(Loop()));
    timer.start(20);
    int error_code = exec();

    if (emulator.isCartLoaded()) { //kill app during emulation
        emulator.unload();
    }

    if (!config.save(app_dir + _CFG_FILE)) {
        Dialog::Warn(lang.get("cfg_save"));
    }

    return error_code;
}

void App::Loop() {
    processEvents(QEventLoop::AllEvents);

    if (emulator.isCartLoaded() && mgui.isActive()) {
        emulator.run();
        processEvents(QEventLoop::AllEvents);
        timer.setInterval(0);
    } else {
        timer.setInterval(10);
        emulator.pollInputDevices();
    }

}

bool App::notify(QObject * receiver, QEvent * event) {
    try {
        return QApplication::notify(receiver, event);
    } catch (const Exception& exception) {
        exception.handle();
        closeAllWindows();
    }
    return false;
}
