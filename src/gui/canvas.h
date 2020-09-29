#ifndef CANVAS_H
#define CANVAS_H

#include <QFrame>

class MGui;

class Canvas : public QFrame
{
    MGui* mgui;
public:
    Canvas(MGui* mgui);
    QPaintEngine* paintEngine() const;
    void paintEvent(QPaintEvent*);

protected:
    void focusInEvent ( QFocusEvent* event );
    void focusOutEvent ( QFocusEvent* event );
};

#endif // CANVAS_H
