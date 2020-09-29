#include "recent.h"
#include "settings.h"

Recent::Recent() {
    settingListId = Settings::findSettingsByName("recent");
    count = (unsigned)settingListId.size();
    for (unsigned i = 0; i < count; i++) {
        settingListData.append ( Settings::get<QString>(settingListId.at(i)) );
    }
    sort();
    save();
}

void Recent::sort() {
    unsigned lastSeted = 0;
    for (unsigned i = 0; i < count; i++) {
        if (settingListData[i] != "") {
            settingListData[lastSeted] = settingListData.at(i);
            if (lastSeted != i) {
                settingListData[i] = "";
            }
            lastSeted++;
        }
    }
}

bool Recent::add(QString name) {
    if (Settings::get<bool>("recent_lock")) return false;
    for(unsigned i = 0; i < count; i++) {
        if (settingListData.at(i) == name) return false;
    }
    for (unsigned i = count-1; i > 0; i--) {
        settingListData[i] = settingListData.at(i - 1);
    }
    settingListData[0] = name;
    return true;
}

void Recent::clear() {
    for(unsigned i = 0; i < count; i++) {
        settingListData[i] = "";
    }
}

void Recent::save() {
    for (unsigned i = 0; i < count; i++) {
        Settings::set<QString>(settingListId.at(i), settingListData.at(i));
    }
}

QStringList Recent::get() {
    return settingListData;
}

unsigned Recent::getCount() {
    return count;
}
