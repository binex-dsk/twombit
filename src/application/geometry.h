#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QRect>

class Geometry
{
    unsigned width;
    unsigned height;
    unsigned frameWidth;
    unsigned frameHeight;
    unsigned spacer;
    unsigned menuHeight;
    QRect deskRect;
    bool dualScreen;
    bool aspectCorrect;

    unsigned displayWidth;
    unsigned screenWidth;
    unsigned displayHeight;
    unsigned screenHeight;
    unsigned centerX;
    unsigned centerY;

    unsigned overflowWidth;
    unsigned overflowHeight;

public:
    Geometry(unsigned w, unsigned h);
    void setWidth(unsigned w);
    void setHeight(unsigned h);
    void setDualScreen(bool state);
    void setAspectCorrect(bool state);
    void setFrameWidth(unsigned w);
    void setFrameHeight(unsigned h);
    void setDesktopRect(QRect rect);
    void setMenuHeight(unsigned h);
    void setSpacer(unsigned w);

    unsigned getScreenWidth();
    unsigned getScreenHeight();
    unsigned getDisplayWidth();
    unsigned getDisplayHeight();
    unsigned getCenterX();
    unsigned getCenterY();

    void calc();
    void center();
};

#endif // GEOMETRY_H
