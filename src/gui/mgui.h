#ifndef MGUI_H
#define MGUI_H

#include <QtGui/QtGui>

class Canvas;
class Lang;
class Resolution;
class CGui;

class MGui : public QMainWindow {
    Q_OBJECT

    QHBoxLayout* display;
    Canvas* canvas;
    Canvas* canvasL;
    Canvas* canvasR;
    QWidget* centralWidget;
    QMenuBar* menu;
    Lang* lang;
    Resolution* resolution;
    CGui* cgui;
    unsigned menuHeight;

    void createCanvas();
    void createMenu();
    void generateRecent();
    void generateResolution();
    void generateSpeed();
    void generateRegion();
    void generateSlot();
    void generateFilter();
    void generateLoadOptions();
    void bindShortcuts();
    void checkMenuItem(QAction* action, bool state, bool displayUncheck=false);
    void _resize(bool backFromFullscreen = false, bool switchMenu = false);
    void closeEvent(QCloseEvent* event);
    void mousePressEvent (QMouseEvent* event );
    QString openFile(QString extension, QString initPath);

    void setSpeedDisplay(int pos);
    void setWinResDisplay(int pos);
    void setFullResDisplay(int pos);
    void setHwFilterDisplay(int pos);
    void setSwFilterDisplay(int pos);
    void setRegionDisplay(int pos);
    void setIcon(QAction* _act, QString _icon);
    void setIcon(QMenu* _menu, QString _icon);

    QSignalMapper* signalMapper_win;
    QSignalMapper* signalMapper_full;
    QSignalMapper* signalMapper_slot;
    QSignalMapper* signalMapper_recent;
    QSignalMapper* signalMapper_speed;
    QSignalMapper* signalMapper_region;
    QSignalMapper* signalMapper_hw_filter;
    QSignalMapper* signalMapper_sw_filter;
    QSignalMapper* signalMapper_open_cfg;
    QSignalMapper* signalMapper_load;

    QMenu* file;
        QAction* file_load_ar[4];
        QAction* file_unload;
        QAction* file_load_state;
        QAction* file_save_state;
        QMenu* file_change_slot;
            QAction* file_change_slot_ar[10];
        QAction* file_power;
        QAction* file_reset;
        QAction* file_cheat_codes;
        QMenu* file_recent_roms;
            QAction* file_recent_roms_ar[10];
            QAction* file_recent_roms_lock;
            QAction* file_recent_roms_clear;
        QAction* file_exit;

    QMenu* video;
        QMenu* video_window_size;
            QAction* video_screen_win_ar[10];
        QMenu* video_fullscreen;
            QAction* video_screen_full_ar[10];
            QAction* video_switch;
        QMenu* video_region;
            QAction* video_region_ar[4];
        QMenu* video_filter;
            QAction* video_filter_hw_ar[2];
            QAction* video_filter_sw_ar[2];
        QAction* video_vsync;
        QAction* video_aspect_correct;
        QAction* video_adjust_video;

    QMenu* config;
        QAction* config_set_input;
        QAction* config_set_paths;
        QAction* config_adjust_audio;
        QAction* config_adjust_options;
        QMenu* config_set_speed;
            QAction* config_set_speed_ar[5];
        QAction* config_show_fps;
        QAction* config_audio_active;
        QAction* config_gear2gear;

    QMenu* help;
        QAction* help_about;

protected:
    void contextMenuEvent( QContextMenuEvent *event);

public:
    MGui(CGui* cgui);
    void initSettings();
    void openUp();
    void create();
    void translate();
    void playMusic();
    void stopMusic();
    bool isFullscreen();
    void recoverFullscreen();
    bool isCartloaded();
    void redraw();
    void activateMenuItems(bool state);
    bool isActive() {
       return isActiveWindow() && !isMinimized();
    }
    WId getWId() { return winId(); }
    WId getWindowWId();
    WId getWindowWIdSecond();
    QWidget* getWidget() { return (QWidget*)canvasL; }
    QWidget* getWidgetSecond() { return (QWidget*)canvasR; }
    void setTitle(QString title);
    void setSlotTs(unsigned pos, unsigned long long ts);

public slots:
    void load(const QString ident);
    void unload_cart();
    void load_state();
    void save_state();
    void change_slot(int pos);
    void change_slot_up();
    void change_slot_down();
    void power();
    void reset();
    void set_region(const int pos);
    void set_vsync();
    void set_aspect_correct();
    void show_fps();
    void audio_active();
    void about();
    void openCfgWindow(const int panel);
    void set_change_win_res(const int pos);
    void set_change_full_res(const int pos);
    void toggle_fullscreen();
    void load_recent(const QString recent);
    void load_command(const QString _fn);
    void recent_lock();
    void recent_clear();
    void set_speed(const int pos);
    void set_sw_filter(const int pos);
    void set_hw_filter(const int pos);
    void gear2gear();
};

#endif

