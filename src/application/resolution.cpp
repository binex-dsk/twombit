#include "resolution.h"
#include "settings.h"
#include <string/qstring.h>
#include <QStringList>

Resolution::Resolution() {
    fullscreen = false;
    winPos = Settings::get<int>("video_mode_window");
    fullPos = Settings::get<int>("video_mode_fullscreen");
    QStringList list = Settings::findSettingsByName("window_mode");
    countWindowRes = (unsigned)list.size();
    for (unsigned i = 0; i < countWindowRes; i++) {
        winList.resize(winList.size() + 1);
        QString data = Settings::get<QString>(list.at(i));
        winList[i] = readRes(data, true);
    }
    list = Settings::findSettingsByName("fullscreen_mode");
    countFullscreenRes = (unsigned)list.size();
    for (unsigned i = 0; i < countFullscreenRes; i++) {
        fullscreenList.resize(fullscreenList.size() + 1);
        QString data = Settings::get<QString>(list.at(i));
        fullscreenList[i] = readRes(data, false);
    }
}

Resolution::_Res Resolution::readRes(QString data, bool isWin) {
    QRegExp rx("[1-9]{1}[0-9]{2,3}");
    bool error = false;
    int result;
    _Res res;
    res.display = data;
    res.width = 800;
    res.height = 600;
    res.fullscreen = isWin ? false : true;
    int pos = 0;
    unsigned c = 0;
    while ((pos = rx.indexIn(data, pos)) != -1) {
        result = __QString::string_to_int( __QString::DEC, rx.cap(0), error);
        if (error) break;

        if (c==0) res.width = result;
        else if (c==1) res.height = result;
        else break;
        c++;
        pos += rx.matchedLength();
    }
    return res;
}

bool Resolution::isRes(QString _data) {
    QRegExp rx("[1-9]{1}[0-9]{2,3}[x][1-9]{1}[0-9]{2,3}");
    if (rx.exactMatch (_data)) return true;
    return false;
}

unsigned Resolution::getWidth() {
    return fullscreen ? fullscreenList[fullPos].width : winList[winPos].width;
}

unsigned Resolution::getHeight() {
    return fullscreen ? fullscreenList[fullPos].height : winList[winPos].height;
}

unsigned Resolution::isFullscreen() {
    return fullscreen;
}

void Resolution::setFullscreen(bool state) {
    fullscreen = state;
}

void Resolution::toggleFullscreen() {
    fullscreen ^= 1;
}

void Resolution::setPos(unsigned pos, bool fullscreen) {
    if (fullscreen) {
        fullPos = pos;
        Settings::set<int>("video_mode_fullscreen", pos);
    } else {
        winPos = pos;
        Settings::set<int>("video_mode_window", pos);
    }
}

int Resolution::getPos(bool fullscreen) {
    if (fullscreen) {
        return fullPos;
    } else {
        return winPos;
    }
}

QStringList Resolution::getWinList() {
    QStringList list;
    for (unsigned i = 0; i < countWindowRes; i++) {
        list.append(winList[i].display);
    }
    return list;
}

QStringList Resolution::getFullList() {
    QStringList list;
    for (unsigned i = 0; i < countFullscreenRes; i++) {
        list.append(fullscreenList[i].display);
    }
    return list;
}

unsigned Resolution::getCountWin() {
    return countWindowRes;
}

unsigned Resolution::getCountFull() {
    return countFullscreenRes;
}
