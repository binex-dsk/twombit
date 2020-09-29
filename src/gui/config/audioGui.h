#ifndef AUDIOGUI_H
#define AUDIOGUI_H

#include <QtGui/QtGui>
class Lang;

class AudioGui : public QObject {
    Q_OBJECT
    Lang* lang;

    QWidget* panel;
    QVBoxLayout* layout;

    QLabel* frequency_label;
    QHBoxLayout* frequency;
    QSlider* frequency_slider;
    QLabel* frequency_counter_label;
    QPushButton* frequency_default;

    QLabel* volume_label;
    QHBoxLayout* volume;
    QSlider* volume_slider;
    QLabel* volume_counter_label;
    QPushButton* volume_default;

public:
    AudioGui();
    void create();
    void translate();
    void init();
    void setVolumeDisplay(int pos);
    void setFrequencyDisplay(int pos);
    QWidget* get_panel() {
        return panel;
    }

public slots:
    void slot_volume_update(int pos);
    void slot_frequency_update(int pos);
    void slot_volume_default();
    void slot_frequency_default();
};

#endif // AUDIOGUI_H
