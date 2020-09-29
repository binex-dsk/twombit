#ifndef VIDEOGUI_H
#define VIDEOGUI_H

#include <QtGui/QtGui>
class Lang;

class VideoGui : public QObject {
    Q_OBJECT
    Lang* lang;
    QWidget* panel;
    QVBoxLayout* layout;

    QLabel* brightness_label;
    QHBoxLayout* brightness;
    QSlider* brightness_slider;
    QLabel* brightness_counter_label;
    QPushButton* brightness_default;

    QLabel* contrast_label;
    QHBoxLayout* contrast;
    QSlider* contrast_slider;
    QLabel* contrast_counter_label;
    QPushButton* contrast_default;

    QLabel* gamma_label;
    QHBoxLayout* gamma;
    QSlider* gamma_slider;
    QLabel* gamma_counter_label;
    QPushButton* gamma_default;

    QLabel* scanline_label;
    QHBoxLayout* scanline;
    QSlider* scanline_slider;
    QLabel* scanline_counter_label;
    QCheckBox* scanline_switch;

    QLabel* ntsc_label;
    QHBoxLayout* ntsc;
    QButtonGroup* ntsc_group;
    QRadioButton* av_radio;
    QRadioButton* svideo_radio;
    QRadioButton* rgb_radio;

    void setBrightnessDisplay(int pos);
    void setContrastDisplay(int pos);
    void setGammaDisplay(int pos);
    void setScanlineDisplay(int pos);
    void setScanlineSwitchDisplay(bool state);

public:
    VideoGui();
    void create();
    void translate();
    void init();
    QWidget* get_panel() {
        return panel;
    }

public slots:
    void slot_brightness_update(int pos);
    void slot_brightness_default();
    void slot_contrast_update(int pos);
    void slot_contrast_default();
    void slot_gamma_update(int pos);
    void slot_gamma_default();
    void slot_scanline_update(int pos);
    void slot_scanline_switch(bool state);

    void slot_ntsc_av_update();
    void slot_ntsc_svideo_update();
    void slot_ntsc_rgb_update();
};

#endif // VIDEOGUI_H
