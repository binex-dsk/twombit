#ifndef RECENT_H
#define RECENT_H

#include <QStringList>

class Recent
{
    QStringList settingListId;
    QStringList settingListData;
    unsigned count;

public:
    Recent();
    bool add(QString name);
    void clear();
    void save();
    QStringList get();
    unsigned getCount();
    void sort();
};

#endif // RECENT_H
