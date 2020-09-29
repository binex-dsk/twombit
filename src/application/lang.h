#ifndef LANG_H
#define LANG_H

#include <QString>
#include <QtXml/QDomDocument>

class Lang
{
    QString dir;
    QString file;
    QDomDocument doc;
    bool xmlReaded;
    void readXML();
    static Lang* instance;

public:
    static Lang* getInstance() {
        return instance;
    }

    QString get(QString id);
    void setDir(QString _dir);
    void setFile(QString _file);
    QString getCompletePath();
    bool isStandardLang();
    Lang();
    bool checkXML(QString textXMLFile);
};

#endif // LANG_H
