
#include <QFile>
#include "lang.h"
#include "exception.h"
#include "dialog.h"

Lang* Lang::instance = 0;

Lang::Lang() {
    instance = this;
    xmlReaded = false;
}

void Lang::setDir(QString _dir) {
    dir = _dir;
}

void Lang::setFile(QString _file) {
    file = _file;
    xmlReaded = false;
}

QString Lang::getCompletePath() {
    return dir + _LANG_DIR + file;
}

QString Lang::get(QString id) {
    if (!xmlReaded) readXML();
    QString buffer;
    QDomElement root = doc.documentElement();
    QDomNode node =  root.firstChildElement();

    while(!node.isNull()) {
        if (node.firstChildElement("id").text().toLower() == id.toLower()) {
            buffer = node.firstChildElement("body").text();
            break;
        }
        node = node.nextSibling();
    }

    if (buffer.length() == 0) {
        return id;
    }
    return buffer;
}

void Lang::readXML() {
    QFile xmlfile( getCompletePath() );

    if(!xmlfile.open( QIODevice::ReadOnly )) {
        if ( isStandardLang() ) {
            throw Exception("standard language plugin not found");
        } else {
            Dialog::Warn("language plugin not found, try switching to standard language");
            setFile(_LANG_FILE);
            return readXML();
        }
    }

    if (!doc.setContent(&xmlfile,true)) {
        xmlfile.close();
        throw Exception("language file is corrupt");
    }
    xmlfile.close();
    xmlReaded = true;
}

bool Lang::checkXML(QString textXMLFile) {
    QFile xmlfile( dir + _LANG_DIR + textXMLFile );

    if(!xmlfile.open( QIODevice::ReadOnly )) return false;
    QDomDocument testDoc;
    if (!testDoc.setContent(&xmlfile,true)) {
        xmlfile.close();
        return false;
    }
    xmlfile.close();
    return true;
}

bool Lang::isStandardLang() {
    return file == _LANG_FILE;
}

