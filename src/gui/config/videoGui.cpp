#include "videoGui.h"
#include "../../application/lang.h"
#include "../../application/settings.h"
#include "../../application/emulator.h"

VideoGui::VideoGui() {
    lang = Lang::getInstance();
    panel = 0;
}

void VideoGui::create() {
    panel = new QWidget;
    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    brightness_label = new QLabel("");
    layout->addWidget(brightness_label);
    brightness = new QHBoxLayout;
    brightness->setMargin(0);
    brightness_slider = new QSlider(Qt::Horizontal);
    brightness_slider->setRange(-100, 100);
    brightness->addWidget(brightness_slider);
    brightness_counter_label = new QLabel("");
    brightness_counter_label->setFixedWidth(25);
    brightness->addWidget(brightness_counter_label);
    brightness_default = new QPushButton("");
    brightness->addWidget(brightness_default);
    brightness->setSpacing(5);
    layout->addLayout(brightness);
    layout->addSpacing(5);

    contrast_label = new QLabel("");
    layout->addWidget(contrast_label);
    contrast = new QHBoxLayout;
    contrast->setMargin(0);
    contrast_slider = new QSlider(Qt::Horizontal);
    contrast_slider->setRange(-100, 100);
    contrast->addWidget(contrast_slider);
    contrast_counter_label = new QLabel("");
    contrast_counter_label->setFixedWidth(25);
    contrast->addWidget(contrast_counter_label);
    contrast_default = new QPushButton("");
    contrast->addWidget(contrast_default);
    contrast->setSpacing(5);
    layout->addLayout(contrast);
    layout->addSpacing(5);

    gamma_label = new QLabel("");
    layout->addWidget(gamma_label);
    gamma = new QHBoxLayout;
    gamma->setMargin(0);
    gamma_slider = new QSlider(Qt::Horizontal);
    gamma_slider->setRange(50, 350);
    gamma->addWidget(gamma_slider);
    gamma_counter_label = new QLabel("");
    gamma_counter_label->setFixedWidth(25);
    gamma->addWidget(gamma_counter_label);
    gamma_default = new QPushButton("");
    gamma->addWidget(gamma_default);
    gamma->setSpacing(5);
    layout->addLayout(gamma);
    layout->addSpacing(5);

    scanline_label = new QLabel("Scanlines:");
    layout->addWidget(scanline_label);
    scanline = new QHBoxLayout;
    scanline ->setMargin(0);
    scanline_slider = new QSlider(Qt::Horizontal);
    scanline_slider->setRange(0, 100);
    scanline->addWidget(scanline_slider);
    scanline_counter_label = new QLabel("");
    scanline_counter_label->setFixedWidth(25);
    scanline->addWidget(scanline_counter_label);
    scanline_switch = new QCheckBox();

    scanline->addWidget(scanline_switch);
    scanline->setSpacing(5);
    layout->addLayout(scanline);
    layout->addSpacing(10);

    QFrame *f = new QFrame( );
    f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    layout->addWidget( f, 0 );
    layout->addSpacing(10);

    ntsc_label = new QLabel("Ntsc Filter:");
    layout->addWidget(ntsc_label);
    layout->addSpacing(5);

    ntsc = new QHBoxLayout;
    ntsc_group = new QButtonGroup(panel);

    av_radio = new QRadioButton("AV");
    ntsc_group->addButton(av_radio);
    ntsc->addWidget(av_radio);

    svideo_radio = new QRadioButton("SVideo");
    ntsc_group->addButton(svideo_radio);
    ntsc->addWidget(svideo_radio);

    rgb_radio = new QRadioButton("RGB");
    ntsc_group->addButton(rgb_radio);
    ntsc->addWidget(rgb_radio);

    layout->addLayout(ntsc);
    layout->addSpacing(5);

    panel->setLayout(layout);
    panel->setMinimumWidth(400);

    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer);

    connect(scanline_switch, SIGNAL(toggled(bool)), this, SLOT(slot_scanline_switch(bool)));
    connect(scanline_slider, SIGNAL(valueChanged(int)), this, SLOT(slot_scanline_update(int)));
    connect(brightness_slider, SIGNAL(valueChanged(int)), this, SLOT(slot_brightness_update(int)));
    connect(brightness_default, SIGNAL(released()), this, SLOT(slot_brightness_default()));
    connect(contrast_slider, SIGNAL(valueChanged(int)), this, SLOT(slot_contrast_update(int)));
    connect(contrast_default, SIGNAL(released()), this, SLOT(slot_contrast_default()));
    connect(gamma_slider, SIGNAL(valueChanged(int)), this, SLOT(slot_gamma_update(int)));
    connect(gamma_default, SIGNAL(released()), this, SLOT(slot_gamma_default()));

    connect(av_radio, SIGNAL(pressed()), this, SLOT(slot_ntsc_av_update()));
    connect(svideo_radio, SIGNAL(pressed()), this, SLOT(slot_ntsc_svideo_update()));
    connect(rgb_radio, SIGNAL(pressed()), this, SLOT(slot_ntsc_rgb_update()));

    translate();
}

void VideoGui::init() {
    setBrightnessDisplay(Settings::get<int>("video_brightness"));
    setContrastDisplay(Settings::get<int>("video_contrast"));
    setGammaDisplay(Settings::get<int>("video_gamma"));
    setScanlineDisplay(Settings::get<int>("video_scanline_intensity"));
    setScanlineSwitchDisplay(Settings::get<bool>("video_scanlines"));

    int pos = Settings::get<int>("ntsc_filter");
    av_radio->setChecked(pos == 0);
    svideo_radio->setChecked(pos == 1);
    rgb_radio->setChecked(pos == 2);
}

void VideoGui::translate() {
    brightness_label->setText(lang->get("brightness"));
    contrast_label->setText(lang->get("contrast"));
    gamma_label->setText(lang->get("gamma"));
    brightness_default->setText(lang->get("default"));
    contrast_default->setText(lang->get("default"));
    gamma_default->setText(lang->get("default"));
}

void VideoGui::setBrightnessDisplay(int pos) {
    brightness_slider->setSliderPosition(pos);
    brightness_counter_label->setText(QString::number(pos));
}

void VideoGui::setContrastDisplay(int pos) {
    contrast_slider->setSliderPosition(pos);
    contrast_counter_label->setText(QString::number(pos));
}

void VideoGui::setGammaDisplay(int pos) {
    gamma_slider->setSliderPosition(pos);
    gamma_counter_label->setText(QString::number(pos));
}

//slots
void VideoGui::slot_brightness_update(int pos) {
    Settings::set<int>("video_brightness", pos);
    setBrightnessDisplay(pos);
    Emulator::getInstance()->updateColorTable();
}

void VideoGui::slot_brightness_default() {
    int pos = Settings::setInit("video_brightness");
    slot_brightness_update(pos);
}

void VideoGui::slot_contrast_update(int pos) {
    Settings::set<int>("video_contrast", pos);
    setContrastDisplay(pos);
    Emulator::getInstance()->updateColorTable();
}

void VideoGui::slot_contrast_default() {
    int pos = Settings::setInit("video_contrast");
    slot_contrast_update(pos);
}

void VideoGui::slot_gamma_update(int pos) {
    Settings::set<int>("video_gamma", pos);
    setGammaDisplay(pos);
    Emulator::getInstance()->updateColorTable();
}

void VideoGui::slot_gamma_default() {
    int pos = Settings::setInit("video_gamma");
    slot_gamma_update(pos);
}

void VideoGui::setScanlineDisplay(int pos) {
    scanline_slider->setSliderPosition(pos);
    scanline_counter_label->setText(QString::number(pos));
}

void VideoGui::slot_scanline_update(int pos) {
    Settings::set<int>("video_scanline_intensity", pos);
    setScanlineDisplay(pos);
    Emulator::getInstance()->updateScanlines();
}

void VideoGui::setScanlineSwitchDisplay(bool state) {
    scanline_switch->setCheckState(state ? Qt::Checked : Qt::Unchecked);
    scanline_slider->setDisabled(state ? false : true);
}

void VideoGui::slot_scanline_switch(bool state) {
    Settings::set<bool>("video_scanlines", state);
    setScanlineSwitchDisplay(state);
    Emulator::getInstance()->updateScanlines();
}

void VideoGui::slot_ntsc_av_update() {
    Settings::set<int>("ntsc_filter", 0);
    Emulator::getInstance()->updateSwFilter();
}

void VideoGui::slot_ntsc_svideo_update() {
    Settings::set<int>("ntsc_filter", 1);
    Emulator::getInstance()->updateSwFilter();
}

void VideoGui::slot_ntsc_rgb_update() {
    Settings::set<int>("ntsc_filter", 2);
    Emulator::getInstance()->updateSwFilter();
}
