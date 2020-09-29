#include "settings.h"
#include "exception.h"

QVector<Settings*> Settings::list;
unsigned Settings::vector_pos = 0;

Settings::Settings (QString _name, QString _desc, int _data, Type _type, int _minLimit, int _maxLimit ) {
    name = _name;
    desc = _desc;
    data = _data;
    init = _data;	//default
    type = _type;
    minLimit = _minLimit;
    maxLimit = _maxLimit;
    saveable = true;
    listSetting();
}

Settings::Settings(QString _name, QString _desc, QString _data) {
    name	= _name;
    desc	= _desc;
    str_data    = _data;
    str_init    = _data;	//default
    type	= STR;
    saveable = true;
    listSetting();
}

Settings::Settings(QString _name, int _data) {
    name = _name;
    data = _data;
    init = _data;	//default
    saveable = false;
    listSetting();
}

void Settings::listSetting() {
    if (vector_pos >= (unsigned)list.size()) {
        list.resize(list.size() + 20);
    }
    list[vector_pos++] = this;
}

Settings* Settings::findSetting(QString name) {
    for (unsigned z = 0; z < vector_pos; z++) {
        if ((*Settings::list[z]).name == name) {
            return Settings::list[z];
        }
    }
    throw Exception("Setting name: " + name + " is not found");
}

QStringList Settings::findSettingsByName(QString name) {
    QStringList list;
    QRegExp rx( ".*" + name + ".*" );
    for (unsigned z = 0; z < vector_pos; z++) {
        if ((*Settings::list[z]).type != STR) continue;
        if (rx.exactMatch ( (*Settings::list[z]).name )) {
            list.append((*Settings::list[z]).name);
        }
    }
    return list;
}

void Settings::toggle(QString name) {
    Settings* setting = findSetting(name);
    if (setting->type != BIN) return;
    setting->toggle();
}

int Settings::setInit(QString name) {
    Settings* setting = findSetting(name);
    if (setting->type != DEC) return 0;
    return setting->set_init();
}
