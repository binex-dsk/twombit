
#include "mgui.h"
#include "../application/main.h"
#include "../application/lang.h"
#include "../application/recent.h"
#include "../application/resolution.h"
#include "../application/settings.h"
#include "canvas.h"
#include "cgui.h"
#include "../application/emulator.h"
#include "../application/file.h"
#include "../application/app.h"
#include "../application/dialog.h"
#include "../application/geometry.h"

MGui::MGui(CGui* cgui)
: cgui(cgui)
{
    lang = Lang::getInstance();
    signalMapper_recent = 0;
}

void MGui::closeEvent(QCloseEvent* event) {
   cgui->close();
}

void MGui::mousePressEvent ( QMouseEvent* event ) {
    if( event->button() == Qt::LeftButton ) {
        Emulator::getInstance()->inputAcquire();
    }
}

WId MGui::getWindowWId() { return canvasL->winId(); }
WId MGui::getWindowWIdSecond() { return canvasR->winId(); }

void MGui::create() {
    setWindowTitle(_PRG " " _VER);
    setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowMaximizeButtonHint);
    createCanvas();
    createMenu();
    generateRecent();
    generateResolution();
    generateSpeed();
    generateRegion();
    generateSlot();
    generateFilter();
    translate();
    bindShortcuts();
    activateMenuItems(false);
    #if defined(Q_WS_WIN)
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,FALSE,NULL,TRUE);
    #endif
}

void MGui::initSettings() {
    checkMenuItem(config_show_fps, Settings::get<bool>("show_fps"), true);
    checkMenuItem(file_recent_roms_lock, Settings::get<bool>("recent_lock"), true);
    checkMenuItem(config_audio_active, Settings::get<bool>("audio_active"), true);
    checkMenuItem(video_vsync, Settings::get<bool>("video_vsync"), true);
    checkMenuItem(video_aspect_correct, Settings::get<bool>("video_aspect_correct"), true);
    change_slot(Settings::get<int>("save_slot"));
    setRegionDisplay(Settings::get<int>("video_region"));
    setWinResDisplay(resolution->getPos(false));
#ifdef __WIN32__
    setFullResDisplay(resolution->getPos(true));
#endif
    setSpeedDisplay(Settings::get<int>("speed_select"));
    setSwFilterDisplay(Settings::get<int>("video_filter_sw"));
    setHwFilterDisplay(Settings::get<int>("video_filter_hw"));
}

void MGui::checkMenuItem(QAction* action, bool state, bool displayUncheck) {
    action->setCheckable(true);
    if (state) {
        //action->setIcon(QIcon(":/check.png"));
        action->setChecked(true);
    } else {
        if (displayUncheck) {
            //action->setIcon(QIcon(":/uncheck.png"));
        } else {
            //action->setIcon(QIcon(""));
        }
        action->setChecked(false);
    }
}

void MGui::translate() {
    QVector<Emulator::Models> _models = Emulator::getModels();
    for (int i = 0; i < _models.size(); i++) {
        file_load_ar[i]->setText(lang->get("load") + " " + _models[i].desc);
    }
    file->setTitle(lang->get("file"));
    file_unload->setText(lang->get("unload"));
    file_load_state->setText(lang->get("load_state"));
    file_save_state->setText(lang->get("save_state"));
    file_change_slot->setTitle(lang->get("change_slot"));
    file_cheat_codes->setText(lang->get("cheat_codes"));
    file_power->setText(lang->get("power"));
    file_reset->setText(lang->get("reset"));
    file_recent_roms->setTitle(lang->get("recent_roms"));
    file_exit->setText(lang->get("exit"));
    video->setTitle(lang->get("video"));
    video_window_size->setTitle(lang->get("window_size"));
    video_fullscreen->setTitle(lang->get("fullscreen"));
    video_region->setTitle(lang->get("region"));
    video_filter->setTitle(lang->get("video_filter"));
    video_aspect_correct->setText(lang->get("aspect_ratio"));
    video_adjust_video->setText(lang->get("adjust_video"));
    config->setTitle(lang->get("config"));
    config_set_input->setText(lang->get("input"));
    config_set_paths->setText(lang->get("paths"));
    config_adjust_audio->setText(lang->get("adjust_audio"));
    config_adjust_options->setText(lang->get("options"));
    config_set_speed->setTitle(lang->get("set_speed"));
    config_show_fps->setText(lang->get("show_fps"));
    config_audio_active->setText(lang->get("audio_active"));
    help->setTitle(lang->get("help"));
    help_about->setText(lang->get("about"));
    video_switch->setText(lang->get("fullscreen") + " (Alt+Enter)");
    file_recent_roms_lock->setText(lang->get("lock"));
    file_recent_roms_clear->setText(lang->get("clear"));
}

void MGui::createCanvas() {
    display = new QHBoxLayout;
    display->setSpacing(0);
    display->setContentsMargins(0,0,0,0);
    display->setMargin(0);
    display->setStretch(0,0);

    canvasL = new Canvas(this);
    canvasL->setStyleSheet("QWidget { background-color: black;}");
    canvasL->setFocusPolicy ( Qt::StrongFocus );
    //canvasL->setAttribute(Qt::WA_PaintOnScreen, false);
    canvasL->setFrameShadow(QFrame::Plain);
    canvasL->setLineWidth(0);
    canvasL->setFrameStyle ( QFrame::Panel );

    canvasR = new Canvas(this);
    canvasR->setStyleSheet("QWidget { background-color: black;}");
    canvasR->setFocusPolicy ( Qt::StrongFocus );
    //canvasR->setAttribute(Qt::WA_PaintOnScreen, false);
    canvasR->setFrameShadow(QFrame::Plain);
    canvasR->setLineWidth(0);
    canvasR->setFrameStyle ( QFrame::Panel );
    canvasR->setFixedSize(0, 0);

    display->addWidget(canvasL);
    display->addWidget(canvasR);

    centralWidget = new QWidget;
    centralWidget->setLayout(display);
    setCentralWidget(centralWidget);
}

void MGui::bindShortcuts() {
    new QShortcut ( QKeySequence("Alt+Return"), this, SLOT( toggle_fullscreen() ));
    new QShortcut ( QKeySequence("Escape"), this, SLOT( toggle_fullscreen() ));
    new QShortcut ( QKeySequence("F5"), this, SLOT( save_state() ));
    new QShortcut ( QKeySequence("F6"), this, SLOT( change_slot_up() ));
    new QShortcut ( QKeySequence("F7"), this, SLOT( change_slot_down() ));
    new QShortcut ( QKeySequence("F8"), this, SLOT( load_state() ));
}

void MGui::activateMenuItems(bool state) {
    file_unload->setEnabled( state );
    file_power->setEnabled( state );
    file_reset->setEnabled( state );
    file_cheat_codes->setEnabled( state );
    bool g2g = Settings::get<int>("gg_gear_to_gear") == 1;
    file_load_state->setEnabled( g2g ? false : state );
    file_save_state->setEnabled( g2g ? false : state );
    file_change_slot->setEnabled( g2g ? false : state );
    video_region_ar[0]->setEnabled( !state );
    video_region_ar[1]->setEnabled( !state );
    video_region_ar[2]->setEnabled( !state );
    video_region_ar[3]->setEnabled( !state );
    config_gear2gear->setEnabled( !state );
}

void MGui::setIcon(QAction* _act, QString _icon) {
#ifdef __WIN32__
    _act->setIcon(QIcon(_icon));
#endif
}

void MGui::setIcon(QMenu* _menu, QString _icon) {
#ifdef __WIN32__
    _menu->setIcon(QIcon(_icon));
#endif
}


void MGui::createMenu() {
    signalMapper_open_cfg = new QSignalMapper(this);
    menu = menuBar();

    file = menu->addMenu("");
    generateLoadOptions();
    file_unload = file->addAction("");
    setIcon(file_unload, ":/unload.png");
    //file_unload->setIcon(QIcon(":/unload.png"));
    connect(file_unload, SIGNAL(triggered()), this, SLOT(unload_cart()));
    file->addSeparator();
    file_load_state = file->addAction("");
    //file_load_state->setIcon(QIcon(":/load_state.png"));
    setIcon(file_load_state, ":/load_state.png");
    connect(file_load_state, SIGNAL(triggered()), this, SLOT(load_state()));
    file_save_state = file->addAction("");
    //file_save_state->setIcon(QIcon(":/save_state.png"));
    setIcon(file_save_state, ":/save_state.png");
    connect(file_save_state, SIGNAL(triggered()), this, SLOT(save_state()));
    file_change_slot = file->addMenu("");
    //file_change_slot->setIcon(QIcon(":/change_slot.png"));
    setIcon(file_change_slot, ":/change_slot.png");
    file->addSeparator();
    file_cheat_codes = file->addAction("");
    //file_cheat_codes->setIcon(QIcon(":/cheat.png"));
    setIcon(file_cheat_codes, ":/cheat.png");
        signalMapper_open_cfg->setMapping(file_cheat_codes, CGui::p_cheats);
        connect(file_cheat_codes, SIGNAL(triggered()),signalMapper_open_cfg, SLOT (map()));
    file->addSeparator();
    file_power = file->addAction("");
    //file_power->setIcon(QIcon(":/power.png"));
    setIcon(file_power, ":/power.png");
    connect(file_power, SIGNAL(triggered()), this, SLOT(power()));
    file_reset = file->addAction("");
    //file_reset->setIcon(QIcon(":/reset.png"));
    setIcon(file_reset, ":/reset.png");
    connect(file_reset, SIGNAL(triggered()), this, SLOT(reset()));
    file->addSeparator();
    file_recent_roms = file->addMenu("");
    //file_recent_roms->setIcon(QIcon(":/recent.png"));
    setIcon(file_recent_roms, ":/recent.png");
    file->addSeparator();
    file_exit = file->addAction("");
    //file_exit->setIcon(QIcon(":/exit.png"));
    setIcon(file_exit, ":/exit.png");
    connect(file_exit, SIGNAL(triggered()), this, SLOT(close()));

    video = menu->addMenu("");
    video_window_size = video->addMenu("");
    //video_window_size->setIcon(QIcon(":/screen.png"));
    setIcon(video_window_size, ":/screen.png");
    video_fullscreen = video->addMenu("");
    //video_fullscreen->setIcon(QIcon(":/screen.png"));
    setIcon(video_fullscreen, ":/screen.png");
    video->addSeparator();
    video_region = video->addMenu("");
    //video_region->setIcon(QIcon(":/region.png"));
    setIcon(video_region, ":/region.png");
    video_filter = video->addMenu("");
    //video_filter->setIcon(QIcon(":/filter.png"));
    setIcon(video_filter, ":/filter.png");
    video_vsync = video->addAction("Vsync");
    connect(video_vsync, SIGNAL(triggered()),this, SLOT (set_vsync()));
    video_aspect_correct = video->addAction("");
    connect(video_aspect_correct, SIGNAL(triggered()),this, SLOT (set_aspect_correct()));
    video_adjust_video = video->addAction("");
    //video_adjust_video->setIcon(QIcon(":/sun.png"));
    setIcon(video_adjust_video, ":/sun.png");
        signalMapper_open_cfg->setMapping(video_adjust_video, CGui::p_video);
        connect(video_adjust_video, SIGNAL(triggered()),signalMapper_open_cfg, SLOT (map()));

    config = menu->addMenu("");
    config_set_input = config->addAction("");
    //config_set_input->setIcon(QIcon(":/joypad.png"));
    setIcon(config_set_input, ":/joypad.png");
        signalMapper_open_cfg->setMapping(config_set_input, CGui::p_input);
        connect(config_set_input, SIGNAL(triggered()),signalMapper_open_cfg, SLOT (map()));
    config_set_paths = config->addAction("");
        //config_set_paths->setIcon(QIcon(":/folder.png"));
        setIcon(config_set_paths, ":/folder.png");
        signalMapper_open_cfg->setMapping(config_set_paths, CGui::p_paths);
        connect(config_set_paths, SIGNAL(triggered()),signalMapper_open_cfg, SLOT (map()));
    config_adjust_audio = config->addAction("");
        //config_adjust_audio->setIcon(QIcon(":/audio.png"));
        setIcon(config_adjust_audio, ":/audio.png");
        signalMapper_open_cfg->setMapping(config_adjust_audio, CGui::p_audio);
        connect(config_adjust_audio, SIGNAL(triggered()),signalMapper_open_cfg, SLOT (map()));
    config_adjust_options = config->addAction("");
        //config_adjust_options->setIcon(QIcon(":/options.png"));
        setIcon(config_adjust_options, ":/options.png");
        signalMapper_open_cfg->setMapping(config_adjust_options, CGui::p_options);
        connect(config_adjust_options, SIGNAL(triggered()),signalMapper_open_cfg, SLOT (map()));
    config_set_speed = config->addMenu("");
    //config_set_speed->setIcon(QIcon(":/speed.png"));
    setIcon(config_set_speed, ":/speed.png");
    config_show_fps = config->addAction("");
    connect(config_show_fps, SIGNAL(triggered()),this, SLOT (show_fps()));
    config_audio_active = config->addAction("");
    connect(config_audio_active, SIGNAL(triggered()),this, SLOT (audio_active()));
    config_gear2gear = config->addAction("Gear 2 Gear");
    connect(config_gear2gear, SIGNAL(triggered()),this, SLOT (gear2gear()));

    help = menu->addMenu("");
    help_about = help->addAction("");
    //help_about->setIcon(QIcon(":/about.png"));
    setIcon(help_about, ":/about.png");
    connect(help_about, SIGNAL(triggered()), this, SLOT(about()));

    connect(signalMapper_open_cfg, SIGNAL(mapped(const int)), this, SLOT(openCfgWindow(const int)));

}

void MGui::generateResolution() {
    resolution = new Resolution();
    QStringList list = resolution->getWinList();
    signalMapper_win = new QSignalMapper(this);

    for (unsigned i=0; i<resolution->getCountWin(); i++) {
        video_screen_win_ar[i] = video_window_size->addAction(list[i]);
        signalMapper_win->setMapping(video_screen_win_ar[i], i);
        connect(video_screen_win_ar[i], SIGNAL(triggered()),signalMapper_win, SLOT (map()));
    }
    connect(signalMapper_win, SIGNAL(mapped(const int)), this, SLOT(set_change_win_res(const int)));
#ifdef __WIN32__
    list = resolution->getFullList();
    signalMapper_full = new QSignalMapper(this);

    for (unsigned i=0; i<resolution->getCountFull(); i++) {
        video_screen_full_ar[i] = video_fullscreen->addAction(list[i]);
        signalMapper_full->setMapping(video_screen_full_ar[i], i);
        connect(video_screen_full_ar[i], SIGNAL(triggered()),signalMapper_full, SLOT (map()));
    }
    connect(signalMapper_full, SIGNAL(mapped(const int)), this, SLOT(set_change_full_res(const int)));
#endif
    video_fullscreen->addSeparator();
    video_switch = video_fullscreen->addAction("");
    //video_switch->setIcon(QIcon(":/screen.png"));
    setIcon(video_switch, ":/screen.png");
    connect(video_switch, SIGNAL(triggered()),this, SLOT (toggle_fullscreen()));
}

void MGui::generateRecent() {
    Recent recent;
    QStringList list = recent.get();

    if (signalMapper_recent) {
        delete signalMapper_recent;
        signalMapper_recent = 0;
    }
    signalMapper_recent = new QSignalMapper(this);
    file_recent_roms->clear();

    for (unsigned i=0; i<recent.getCount(); i++) {
        if (list[i] == "") continue;
        file_recent_roms_ar[i] = file_recent_roms->addAction(list[i]);
        //file_recent_roms_ar[i]->setIcon(QIcon(":/arrow.png"));
        setIcon(file_recent_roms_ar[i], ":/arrow.png");
        signalMapper_recent->setMapping(file_recent_roms_ar[i], list[i]);
        connect(file_recent_roms_ar[i], SIGNAL(triggered()),signalMapper_recent, SLOT (map()));
    }
    connect(signalMapper_recent, SIGNAL(mapped(const QString)), this, SLOT(load_recent(const QString)));

    file_recent_roms->addSeparator();
    file_recent_roms_lock = file_recent_roms->addAction("");
    checkMenuItem(file_recent_roms_lock, Settings::get<bool>("recent_lock"), true);
    connect(file_recent_roms_lock, SIGNAL(triggered()),this, SLOT (recent_lock()));

    file_recent_roms_clear = file_recent_roms->addAction("");
    //file_recent_roms_clear->setIcon(QIcon(":/clear.png"));
    setIcon(file_recent_roms_clear, ":/clear.png");
    connect(file_recent_roms_clear, SIGNAL(triggered()),this, SLOT (recent_clear()));
}

void MGui::generateLoadOptions() {
    signalMapper_load = new QSignalMapper(this);
    QVector<Emulator::Models> _models = Emulator::getModels();
    for (int i = 0; i < _models.size(); i++) {
        file_load_ar[i] = file->addAction("");
        file_load_ar[i]->setVisible(_models[i].inUse ? true : false);
        //file_load_ar[i]->setIcon(QIcon(":/load.png"));
        setIcon(file_load_ar[i], ":/load.png");
        signalMapper_load->setMapping( file_load_ar[i], _models[i].ident);
        connect( file_load_ar[i], SIGNAL(triggered()),signalMapper_load, SLOT (map()));
    }
    connect(signalMapper_load, SIGNAL(mapped(const QString)), this, SLOT(load(const QString)));
}

void MGui::generateSpeed() {
    signalMapper_speed = new QSignalMapper(this);
    for (int i=0; i<5; i++) {
        int speed = Settings::get<int>("speed_" + QString::number(i));
        config_set_speed_ar[i] = config_set_speed->addAction( QString::number(speed) + " %" );
        signalMapper_speed->setMapping( config_set_speed_ar[i], i);
        connect( config_set_speed_ar[i], SIGNAL(triggered()),signalMapper_speed, SLOT (map()));
    }
    connect(signalMapper_speed, SIGNAL(mapped(const int)), this, SLOT(set_speed(const int)));
}

void MGui::generateRegion() {
    signalMapper_region = new QSignalMapper(this);

    video_region_ar[0] = video_region->addAction("USA");
    signalMapper_region->setMapping( video_region_ar[0], 0);
    connect( video_region_ar[0], SIGNAL(triggered()),signalMapper_region, SLOT (map()));

    video_region_ar[1] = video_region->addAction("JAPAN");
    signalMapper_region->setMapping( video_region_ar[1], 1);
    connect( video_region_ar[1], SIGNAL(triggered()),signalMapper_region, SLOT (map()));

    video_region_ar[2] = video_region->addAction("EUROPE");
    signalMapper_region->setMapping( video_region_ar[2], 2);
    connect( video_region_ar[2], SIGNAL(triggered()),signalMapper_region, SLOT (map()));

    video_region_ar[3] = video_region->addAction("AUTO");
    signalMapper_region->setMapping( video_region_ar[3], 3);
    connect( video_region_ar[3], SIGNAL(triggered()),signalMapper_region, SLOT (map()));


    connect(signalMapper_region, SIGNAL(mapped(const int)), this, SLOT(set_region(const int)));
}

void MGui::generateSlot() {
    signalMapper_slot = new QSignalMapper(this);
    for (int i=0; i<10; i++) {
        file_change_slot_ar[i] = file_change_slot->addAction(QString::number(i + 1, 10));
        signalMapper_slot->setMapping(file_change_slot_ar[i], i);
        connect(file_change_slot_ar[i], SIGNAL(triggered()),signalMapper_slot, SLOT (map()));
    }
    connect(signalMapper_slot, SIGNAL(mapped(const int)), this, SLOT(change_slot(int)));
}

void MGui::generateFilter() {
    signalMapper_hw_filter = new QSignalMapper(this);

    video_filter_hw_ar[0] = video_filter->addAction("Point");
    signalMapper_hw_filter->setMapping( video_filter_hw_ar[0], 0);
    connect( video_filter_hw_ar[0], SIGNAL(triggered()),signalMapper_hw_filter, SLOT (map()));

    video_filter_hw_ar[1] = video_filter->addAction("Linear");
    signalMapper_hw_filter->setMapping( video_filter_hw_ar[1], 1);
    connect( video_filter_hw_ar[1], SIGNAL(triggered()),signalMapper_hw_filter, SLOT (map()));

    connect(signalMapper_hw_filter, SIGNAL(mapped(const int)), this, SLOT(set_hw_filter(const int)));

    video_filter->addSeparator();

    signalMapper_sw_filter = new QSignalMapper(this);

    video_filter_sw_ar[0] = video_filter->addAction("Normal");
    signalMapper_sw_filter->setMapping( video_filter_sw_ar[0], 0);
    connect( video_filter_sw_ar[0], SIGNAL(triggered()),signalMapper_sw_filter, SLOT (map()));

    video_filter_sw_ar[1] = video_filter->addAction("NTSC");
    signalMapper_sw_filter->setMapping( video_filter_sw_ar[1], 1);
    connect( video_filter_sw_ar[1], SIGNAL(triggered()),signalMapper_sw_filter, SLOT (map()));

    connect(signalMapper_sw_filter, SIGNAL(mapped(const int)), this, SLOT(set_sw_filter(const int)));
}

void MGui::playMusic() {
    Emulator::getInstance()->playSound();
}

void MGui::stopMusic() {
    Emulator::getInstance()->stopSound();
}

bool MGui::isFullscreen() {
    return resolution->isFullscreen();
}

void MGui::recoverFullscreen() {
    _resize();
}

bool MGui::isCartloaded() {
    return Emulator::getInstance()->isCartLoaded();
}

void MGui::redraw() {

}

void MGui::contextMenuEvent( QContextMenuEvent *event ) {
#if defined(__APPLE__) || defined(__linux__)
    return;
#endif
    if (resolution->isFullscreen()) return;
    if (menu->isVisible()) {
        menu->hide();
    } else {
        menu->show();
    }
    _resize(false, true);
}

void MGui::openUp() {
    show();
    menuHeight = menu->height();
    _resize();
}

void MGui::_resize(bool backFromFullscreen, bool switchMenu) {
    unsigned width = resolution->getWidth();
    unsigned height = resolution->getHeight();
    bool fullscreen = resolution->isFullscreen();
    bool aspectCorrect = Settings::get<bool>("video_aspect_correct");

    if (fullscreen) {
        canvasL->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        menu->hide();
        setCursor( QCursor( Qt::BlankCursor ) );
        Emulator::getInstance()->updateVideo(width, height, true);
        showFullScreen();
        return;
    }

#ifndef __WIN32__
    setWindowFlags(Qt::Window);
#endif

    bool forceMenu = false;
    bool show_normal = false;
    if (backFromFullscreen) {
        showNormal();
        setWindowFlags(Qt::Window);
        menu->show();
        forceMenu = true;
        setCursor(Qt::ArrowCursor);
        show_normal = true;
    } else if (!isVisible()) {
        show_normal = true;
    }

    QRect deskRect = QApplication::desktop()->availableGeometry();
    bool g2g = Settings::get<int>("gg_gear_to_gear") == 1;

    Geometry geo(width, height);
    geo.setDualScreen( g2g );
    geo.setAspectCorrect( aspectCorrect );

#ifdef __WIN32__
    geo.setMenuHeight( menu->isVisible() || forceMenu ? menuHeight : 0 );
#elif __linux__
    geo.setMenuHeight(menuHeight);
#endif

    geo.setFrameWidth( frameSize().width() - size().width() );
    geo.setFrameHeight( frameSize().height() - size().height() );
    geo.setDesktopRect( deskRect );
    geo.setSpacer( 10 );
    geo.calc();

#ifdef __WIN32__
    setFixedSize( geo.getDisplayWidth(), geo.getDisplayHeight() );
    canvasL->setFixedSize(geo.getScreenWidth(), geo.getScreenHeight());
    if ( g2g ) {
        canvasR->setFixedSize(geo.getScreenWidth(), geo.getScreenHeight());
        display->setSpacing(10);
    } else {
        canvasR->setFixedSize(0, 0);
        display->setSpacing(0);
    }
#else
    setFixedSize( geo.getDisplayWidth(), geo.getDisplayHeight() );
    canvasL->setFixedSize(geo.getDisplayWidth(), geo.getScreenHeight());
#endif

    if (!switchMenu) {
        geo.center();
        move(geo.getCenterX(), geo.getCenterY());
    }

    if (show_normal) {
        show();
    }

    Emulator::getInstance()->updateVideo(geo.getScreenWidth(), geo.getScreenHeight(), false);
}

QString MGui::openFile(QString extension, QString initPath) {
    QStringList filters;

    filters << extension + File::getSupportedExtensions() + ")";
    filters << "All files (*)";

    return QFileDialog::getOpenFileName(this, "Load Rom", initPath == "" ? App::getDir() : initPath, extension + File::getSupportedExtensions() + ")\n" + "All files (*)");
}

void MGui::setTitle(QString title) {
    setWindowTitle(title);
}

void MGui::setSlotTs(unsigned pos, unsigned long long ts) {
    QString text = QString::number(pos + 1, 10);
    if (ts != 0) {
        QDateTime* date = new QDateTime();
        date->setTime_t(ts);
        text += " - " + date->toString("ddd d. MMMM yyyy - hh:mm:ss");
        delete(date);
    }
    file_change_slot_ar[pos] -> setText ( text );
}

void MGui::load_command(const QString _fn) {
    QString ident = "sms";
    int end = _fn.lastIndexOf(".");
    if (end != -1) {
        ident = _fn.mid(end + 1);
    }
    Emulator::Models model = Emulator::getModelByIdent(ident);
    bool result = Emulator::getInstance()->openRom(_fn, model);
    if (result) {
        Emulator::getInstance()->clearAudio();
        Emulator::getInstance()->clearVideo();
        playMusic();
        activateWindow();
        activateMenuItems(true);
        generateRecent();
        translate();
    }
}

//slots
void MGui::load(const QString ident) {
    Emulator::Models model = Emulator::getModelByIdent(ident);
    QString cur_path = Settings::get<QString>(ident + "_path_rom");
    stopMusic();
    QString fn = openFile(model.extension, cur_path);
    if (fn == "") {
        playMusic();
        activateWindow();
        return;
    }
    bool result = Emulator::getInstance()->openRom(fn, model);
    if (result) {
        Emulator::getInstance()->clearAudio();
        Emulator::getInstance()->clearVideo();
        playMusic();
        activateWindow();
        activateMenuItems(true);
        generateRecent();
        translate();
    }
}

void MGui::unload_cart() {
    Emulator::getInstance()->unload();
    Emulator::getInstance()->clearVideo();
    Emulator::getInstance()->clearAudio();
    stopMusic();
    activateMenuItems(false);
    setWindowTitle(_PRG " " _VER);
}

void MGui::load_state() {
    Emulator::getInstance()->loadState();
}

void MGui::save_state() {
    Emulator::getInstance()->saveState();
}

void MGui::change_slot(int pos) {
    for (int i = 0; i < 10; i++) {
        checkMenuItem(file_change_slot_ar[i], pos == i);
    }
    Emulator::getInstance()->changeSlot(pos);
    Settings::set<int>("save_slot", pos);
}

void MGui::change_slot_up() {
    int slot = Settings::get<int>("save_slot");
    if (++slot > 9) slot = 0;
    change_slot(slot);
}

void MGui::change_slot_down() {
    int slot = Settings::get<int>("save_slot");
    if (--slot < 0) slot = 9;
    change_slot(slot);
}

void MGui::power() {
    Emulator::getInstance()->power();
}

void MGui::reset() {
    Emulator::getInstance()->reset();
}

void MGui::setRegionDisplay(int pos) {
    for (int i = 0; i < 4; i++) {
        checkMenuItem(video_region_ar[i], pos == i);
    }
}

void MGui::set_region(const int pos) {
    setRegionDisplay(pos);
    Settings::set<int>("video_region", pos);
}

void MGui::set_vsync() {
    Settings::toggle("video_vsync");
    checkMenuItem(video_vsync, Settings::get<bool>("video_vsync"), true);
    _resize();
}

void MGui::set_aspect_correct() {
    Settings::toggle("video_aspect_correct");
    checkMenuItem(video_aspect_correct, Settings::get<bool>("video_aspect_correct"), true);
    _resize();
}

void MGui::show_fps() {
    Settings::toggle("show_fps");
    checkMenuItem(config_show_fps, Settings::get<bool>("show_fps"), true);
    Emulator::getInstance()->activateFPS(Settings::get<bool>("show_fps"));
}

void MGui::audio_active() {
    Settings::toggle("audio_active");
    checkMenuItem(config_audio_active, Settings::get<bool>("audio_active"), true);
    Emulator::getInstance()->activateSound(Settings::get<bool>("audio_active"));
}

void MGui::about() {
    Dialog::Inform(lang->get("about_desc"));
}

void MGui::openCfgWindow(const int panel) {
    cgui->_show(static_cast<CGui::Panel>(panel));
}

void MGui::setWinResDisplay(int pos) {
    for (int i = 0; i < 10; i++) {
        checkMenuItem(video_screen_win_ar[i], pos == i);
    }
}

void MGui::set_change_win_res(const int pos) {
    setWinResDisplay(pos);
    resolution->setPos(pos, false);
    _resize();
}

void MGui::setFullResDisplay(int pos) {
    for (int i = 0; i < 10; i++) {
        checkMenuItem(video_screen_full_ar[i], pos == i);
    }
}

void MGui::set_change_full_res(const int pos) {
    setFullResDisplay(pos);
    resolution->setPos(pos, true);
}

void MGui::toggle_fullscreen() {
    bool isFullscreenBefore = resolution->isFullscreen();
    if (!isFullscreenBefore) {
        if (Emulator::getInstance()->isInputAcquired()) {
            Emulator::getInstance()->inputUnacquire();
            return;
        }
    }

    if ( Settings::get<int>("gg_gear_to_gear") ) return;

    if (!Emulator::getInstance()->isCartLoaded()) return;
    resolution->toggleFullscreen();
    _resize(isFullscreenBefore, false);
#ifdef __WIN32__
    Emulator::getInstance()->reinitInput(); //workaround to fix forground input by switching between win and fullscreen
#endif
}

void MGui::load_recent(const QString recent) {
    QStringList parts = recent.split("|");
    Emulator::Models model = Emulator::getModelByIdent(parts[0].trimmed());
    bool result = Emulator::getInstance()->openRom(parts[1].trimmed(), model);
    if (result) {
        Emulator::getInstance()->clearAudio();
        Emulator::getInstance()->clearVideo();
        playMusic();
        activateMenuItems(true);
    }
}

void MGui::recent_lock() {
    Settings::toggle("recent_lock");
    checkMenuItem(file_recent_roms_lock, Settings::get<bool>("recent_lock"), true);
}

void MGui::recent_clear() {
    if (Dialog::Confirm (lang->get("clear_recent"), lang->get("ok"), lang->get("cancel"))) {
        Recent recent;
        recent.clear();
        recent.save();
        generateRecent();
        translate();
    }
}

void MGui::setSpeedDisplay(int pos) {
    for (int i = 0; i < 5; i++) {
        checkMenuItem(config_set_speed_ar[i], pos == i);
    }
}

void MGui::set_speed(const int pos) {
    setSpeedDisplay(pos);
    Settings::set<int>("speed_select", pos);
    Emulator::getInstance()->updateFrequency();
}

void MGui::setSwFilterDisplay(int pos) {
    for (int i = 0; i < 2; i++) {
        checkMenuItem(video_filter_sw_ar[i], pos == i);
    }
}

void MGui::set_sw_filter(const int pos) {
    setSwFilterDisplay(pos);
    Settings::set<int>("video_filter_sw", pos);
    Emulator::getInstance()->updateSwFilter();
}

void MGui::setHwFilterDisplay(int pos) {
    for (int i = 0; i < 2; i++) {
        checkMenuItem(video_filter_hw_ar[i], pos == i);
    }
}

void MGui::set_hw_filter(const int pos) {
    setHwFilterDisplay(pos);
    Settings::set<int>("video_filter_hw", pos);
    Emulator::getInstance()->updateHwFilter();
}

void MGui::gear2gear() {
    int _state = Settings::get<int>("gg_gear_to_gear");
    Settings::set<int>("gg_gear_to_gear", _state == 0 ? 1 : 0);
    checkMenuItem(config_gear2gear, _state == 0 ? 1 : 0, false);
    _resize();
}
