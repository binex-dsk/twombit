#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <QVector>
#include <QString>

class Resolution
{
    struct _Res {
        unsigned width;
        unsigned height;
        QString display;
        bool fullscreen;
    };
    QVector<_Res> winList;
    QVector<_Res> fullscreenList;
    bool fullscreen;
    unsigned winPos;
    unsigned fullPos;

    unsigned countWindowRes;
    unsigned countFullscreenRes;

    _Res readRes(QString data, bool isWin);

public:
    Resolution();
    static bool isRes(QString _data);
    QStringList getWinList();
    QStringList getFullList();
    unsigned getCountWin();
    unsigned getCountFull();
    unsigned getWidth();
    unsigned getHeight();
    unsigned isFullscreen();
    void setFullscreen(bool state);
    void toggleFullscreen();
    void setPos(unsigned pos, bool fullscreen);
    int getPos(bool fullscreen);
};

#endif // RESOLUTION_H
