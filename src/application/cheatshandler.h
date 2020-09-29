#ifndef CHEATSLOADER_H
#define CHEATSLOADER_H

#include <QVector>
#include <QString>

class CheatsHandler
{
    static CheatsHandler* instance;
    enum Mode { ACTION_REPLAY, GAME_GENIE };

public:
    struct Code {
        Mode mode;
        bool enabled;
        unsigned addr;
        char replaceData;
        char originalData;
        QString cheat;
        QString desc;
    };
private:
    QVector< Code > code_list;
    bool rebuild();

public:
    unsigned count() const { return code_list.size(); }
    Code get(unsigned pos) { return code_list[pos]; }

    void toggle(unsigned pos);
    void remove(unsigned pos);

    CheatsHandler();
    static CheatsHandler* getInstance() {
        if (instance == 0) {
            instance = new CheatsHandler();
        }
        return instance;
    }
    bool add(QString cheat, QString des, bool enabled=false);
    bool edit(QString cheat, QString des, unsigned pos);
    bool decode(QString cheat, Code& code);
    QString getPath(QString ident);
    void load(QString fn, QString ident);
    void save(QString fn, QString ident);
};

#endif // CHEATSLOADER_H
