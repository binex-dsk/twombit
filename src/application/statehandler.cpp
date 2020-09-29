#include "statehandler.h"
#include "settings.h"
#include "app.h"

StateHandler* StateHandler::instance = 0;

StateHandler::StateHandler()
: state()
{

}

unsigned char* StateHandler::load(QString fn, QString ident, unsigned pos) {
    QString state_path = getPath(ident);
    QString state_fn = state_path + "/" + fn + ".sa" + QString::number(pos, 10);
    state.unload();
    state.setFile(state_fn);
    state.fetchData();
    if (state.getError() != File::FINE) {
        return 0;
    }
    return state.getData();
}

unsigned StateHandler::getSize() {
    return state.getSize();
}

bool StateHandler::save(QString fn, QString ident, unsigned pos, unsigned char* buffer, unsigned length) {
    QString state_path = getPath(ident);
    QString state_fn = state_path + "/" + fn + ".sa" + QString::number(pos, 10);
    state.unload();
    state.setFile(state_fn);
    state.write(buffer, length);
    if (state.getError() != File::FINE) {
        free_mem(buffer);
        return false;
    }
    free_mem(buffer);
    return true;
}

QString StateHandler::getPath(QString ident) {
    QString _path = "";

    if (ident == "sms") _path = Settings::get<QString>("sms_path_savestate");
    else if (ident == "gg") _path = Settings::get<QString>("gg_path_savestate");
    else if (ident == "sg") _path = Settings::get<QString>("sg_path_savestate");
    if (_path == "") {
        _path = App::getDir() + "savestate";
    }
    return _path;
}
