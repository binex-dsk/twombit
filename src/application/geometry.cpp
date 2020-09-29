#include "geometry.h"

Geometry::Geometry(unsigned w, unsigned h)
{
    setWidth( w );
    setHeight( h );
    dualScreen = false;
    frameWidth = 0;
    frameHeight = 0;
    spacer = 0;
    menuHeight = 0;
    centerX = 0;
    centerY = 0;
}

void Geometry::setWidth(unsigned w) {
    width = w;
}

void Geometry::setHeight(unsigned h) {
    height = h;
}

void Geometry::setDualScreen(bool state) {
    dualScreen = state;
}

void Geometry::setAspectCorrect(bool state) {
    aspectCorrect = state;
}

void Geometry::setFrameWidth(unsigned w) {
    frameWidth = w;
}

void Geometry::setFrameHeight(unsigned h) {
    frameHeight = h;
}

void Geometry::setDesktopRect(QRect rect) {
    deskRect = rect;
}

void Geometry::setSpacer(unsigned w) {
    spacer = w;
}

void Geometry::setMenuHeight(unsigned h) {
    menuHeight = h;
}

unsigned Geometry::getScreenWidth() {
    return screenWidth;
}

unsigned Geometry::getScreenHeight() {
    return screenHeight;
}

unsigned Geometry::getDisplayWidth() {
    return displayWidth;
}

unsigned Geometry::getDisplayHeight() {
    return displayHeight;
}

unsigned Geometry::getCenterX() {
    return centerX;
}

unsigned Geometry::getCenterY() {
    return centerY;
}

void Geometry::calc() {

    displayHeight = height + menuHeight;
    screenHeight = height;

    if (( displayHeight + frameHeight ) > deskRect.height() ) {
        overflowHeight = (displayHeight + frameHeight) - deskRect.height();

        displayHeight -= overflowHeight;
        screenHeight -= overflowHeight;
    }

    if (aspectCorrect) {
        width = double(screenHeight / 3) * 4;
    }

    displayWidth = dualScreen ? (width * 2) + spacer : width;
    screenWidth = width;

    if (( displayWidth + frameWidth ) > deskRect.width() ) {
        overflowWidth = (displayWidth + frameWidth) - deskRect.width();
        if ((overflowWidth % 2) != 0) {
            overflowWidth += 1;
        }

        displayWidth -= overflowWidth;

        if (dualScreen) {
            overflowWidth /= 2;
        }
        screenWidth -= overflowWidth;
    }

}

void Geometry::center() {

    unsigned app_width = displayWidth + frameWidth;
    unsigned app_height = displayHeight + frameHeight;

    if (app_width < deskRect.width()) {
        centerX = (deskRect.width() - app_width) >> 1;
    }

    if (app_height < deskRect.height()) {
        centerY = (deskRect.height() - app_height) >> 1;
    }
}
