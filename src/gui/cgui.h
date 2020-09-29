#ifndef CGUI_H
#define CGUI_H

#include <QtGui/QtGui>
class Lang;
class PathsGui;
class VideoGui;
class AudioGui;
class InputGui;
class CheatsGui;
class OptionsGui;
class MGui;

class CGui : public QWidget {
    Q_OBJECT

    MGui* mgui;
    Lang* lang;
    QHBoxLayout* layout;

    QListWidget* list;
        QListWidgetItem* input;
        QListWidgetItem* cheats;
        QListWidgetItem* paths;
        QListWidgetItem* video;
        QListWidgetItem* audio;
        QListWidgetItem* options;

    QWidget* panel;
    QStackedLayout* panelLayout;

    PathsGui* pathsGui;
    VideoGui* videoGui;
    AudioGui* audioGui;
    InputGui* inputGui;
    CheatsGui* cheatsGui;
    OptionsGui* optionsGui;

    void createList();
    void createConfigArea();

public:
    CGui(MGui* mgui);
    enum Panel { p_input, p_cheats, p_paths, p_video, p_audio, p_options };
    void create();
    void translate();
    void initSettings();
    void _show(Panel panel);
    void translateAll();
    void inputPollingComplete();
    void cheat_gui_refresh();
    void cheat_gui_enable_elements(bool state);

public slots:
    void slot_list_update();
};

#endif // CGUI_H
