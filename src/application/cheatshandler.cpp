#include "cheatshandler.h"
#include "emulator.h"
#include "dialog.h"
#include "lang.h"
#include "app.h"
#include <QRegExp>
#include <QDir>
#include <QTextStream>

CheatsHandler* CheatsHandler::instance = 0;

CheatsHandler::CheatsHandler()
{
}

bool CheatsHandler::decode(QString cheat, Code& code) {
    bool ok;
    QString str;
    cheat = cheat.toLower();
    static const QRegExp rxGG("([0-9a-f]){3}(-)?([0-9a-f]){3}(-)?([0-9a-f]){3}"); //000-000-000
    static const QRegExp rxAR1("00([0-9a-f]){4}(:)?([0-9a-f]){2}"); //000000:00
    static const QRegExp rxAR2("00([0-9a-f]){2}(-)?([0-9a-f]){4}"); //0000-0000
    static const QRegExp rxKF("([0-9a-f]){6}(:)?([0-9a-f]){2}"); //000000:00

    if (rxGG.exactMatch (cheat)) { //game genie
        code.mode = GAME_GENIE;
        cheat = cheat.replace("-", "");
        str = cheat.left(2);
        code.replaceData = str.toUInt(&ok, 16);
        if (!ok) return false;
        str = cheat.mid(5,1);
        char F = str.toUInt(&ok, 16);
        if (!ok) return false;
        str = cheat.mid(2,3);
        unsigned CDE = str.toUInt(&ok, 16);
        if (!ok) return false;
        code.addr = ((F ^ 0xf) << 12) | CDE;
        str = cheat.mid(6,1);
        unsigned GI = str.toUInt(&ok, 16) << 4;
        if (!ok) return false;
        str = cheat.mid(8,1);
        GI |= str.toUInt(&ok, 16);
        if (!ok) return false;
        code.originalData = (((GI >> 2) | (GI << 6)) & 0xff ) ^ 0xba;
        return true;
    } else if (rxAR1.exactMatch (cheat) || rxAR2.exactMatch (cheat)) {
        code.mode = ACTION_REPLAY;
        cheat = cheat.replace("-", "");
        cheat = cheat.replace(":", "");
        str = cheat.mid(6,2);
        code.replaceData = str.toUInt(&ok, 16);
        if (!ok) return false;
        str = cheat.mid(2,4);
        code.addr = str.toUInt(&ok, 16);
        if (!ok) return false;
        return true;
    } else if (rxKF.exactMatch (cheat)) {
        code.mode = GAME_GENIE;
        cheat = cheat.replace(":", "");
        str = cheat.mid(6,2);
        code.replaceData = str.toUInt(&ok, 16);
        if (!ok) return false;
        str = cheat.left(2);
        code.originalData = str.toUInt(&ok, 16);
        if (!ok) return false;
        str = cheat.mid(2,4);
        code.addr = str.toUInt(&ok, 16);
        if (!ok) return false;
        return true;
    }
    return false;
}

bool CheatsHandler::rebuild() {
    bool error = true;
    Emulator::getInstance()->resetCheat();
    for(unsigned i = 0; i < code_list.size(); i++) {
        if (!code_list[i].enabled) {
            continue;
        }
        error = Emulator::getInstance()->setCheat( code_list[i].cheat.toStdString().c_str() );
    }
    return error;
}

void CheatsHandler::toggle(unsigned pos) {
    code_list[pos].enabled ^= 1;
    rebuild();
}

void CheatsHandler::remove(unsigned pos) {
    code_list.erase(code_list.begin() + pos);
    rebuild();
}

bool CheatsHandler::add(QString cheat, QString desc, bool enabled) {
    Code code;
    if( !decode(cheat, code) ) return false;

    unsigned count = code_list.size();
    code_list.resize(count+1);

    code.desc = desc;
    code.cheat = cheat;
    code.enabled = enabled;
    code_list[count] = code;

    return rebuild();
}

bool CheatsHandler::edit(QString cheat, QString desc, unsigned pos) {
    Code code;
    if( !decode(cheat, code) ) return false;

    code.enabled = code_list[pos].enabled;
    code.desc = desc;
    code.cheat = cheat;
    code_list[pos] = code;

    return rebuild();
}

void CheatsHandler::load(QString fn, QString ident) {
    code_list.clear();
    QString cheat_path = getPath(ident);
    QString cheat_fn = cheat_path + "/" + fn + ".cht";
    QString buffer;

    QStringList line;
    QFile f( cheat_fn );

    if(f.open( QIODevice::ReadOnly )) {
        QTextStream in( &f );
        while ( !(buffer = in.readLine()).isNull() ) {
            line = buffer.split("|");

            if (line.size() == 2) {
                add(line[0], line[1], false);
            } else {
                line = buffer.split(0x9);
                if (line.size() == 2) {
                    add(line[0], line[1], false);
                }
            }
            line.clear();
        }
        f.close();
    }
}

void CheatsHandler::save(QString fn, QString ident) {
    QString cheat_path = getPath(ident);
    QString cheat_fn = cheat_path + "/" + fn + ".cht";
    QDir dir;

    if (code_list.size() == 0) {
        QFile::remove(cheat_fn);
    } else {
        if( !dir.mkpath( cheat_path ) ) {
            Dialog::Warn(Lang::getInstance()->get("cheat_save"));
        } else {
            QFile f( cheat_fn );
            if(f.open( QIODevice::WriteOnly )) {
                QTextStream out(&f);
                for (int i=0; i < code_list.size(); i++) {
                    //QString is_active = code_list[i].enabled ? "on" : "off";
                    //out << is_active + "|" + code_list[i].cheat + "|" + code_list[i].desc << endl;
                    out << code_list[i].cheat + "|" + code_list[i].desc << endl;
                }
                f.close();
            } else {
                Dialog::Warn(Lang::getInstance()->get("cheat_save"));
            }
        }
    }
    code_list.clear();
}

QString CheatsHandler::getPath(QString ident) {
    QString _path = "";

    if (ident == "sms") _path = Settings::get<QString>("sms_path_cheat");
    else if (ident == "gg") _path = Settings::get<QString>("gg_path_cheat");
    else if (ident == "sg") _path = Settings::get<QString>("sg_path_cheat");
    if (_path == "") {
        _path = App::getDir() + "cheat";
    }
    return _path;
}

