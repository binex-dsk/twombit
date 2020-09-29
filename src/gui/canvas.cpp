#include "canvas.h"
#include "mgui.h"

Canvas::Canvas(MGui* mgui)
    : mgui(mgui)
{
//    setAutoFillBackground(false);
//    setAttribute(Qt::WA_OpaquePaintEvent);
//    setAttribute(Qt::WA_NoSystemBackground);
//    setAttribute(Qt::WA_NativeWindow);
//    setAttribute(Qt::WA_PaintOnScreen, true);
//    setAttribute(Qt::WA_StyledBackground, false);
//    setAttribute(Qt::WA_PaintUnclipped);

    setAttribute(Qt::WA_PaintOnScreen);
    //setAttribute(Qt::WA_NoBackground);
}

QPaintEngine* Canvas::paintEngine() const {
    if(mgui->isCartloaded()) {
        mgui->redraw();
        return 0;
    }
    return QWidget::paintEngine();
}

void Canvas::paintEvent(QPaintEvent* event) {
    event->ignore();
}

void Canvas::focusInEvent ( QFocusEvent* event ) {
    if ((event->reason() == Qt::ActiveWindowFocusReason) && (event->type() == QEvent::FocusIn)) {
        if (mgui->isFullscreen()) { //recover
            mgui->recoverFullscreen();
        }
        mgui->playMusic();
    }
}

void Canvas::focusOutEvent ( QFocusEvent* event ) {
    if ((event->reason() == Qt::ActiveWindowFocusReason) && (event->type() == QEvent::FocusOut)) {
        mgui->stopMusic();
    }
}
