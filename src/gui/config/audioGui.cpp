#include "audioGui.h"
#include "../../application/lang.h"
#include "../../application/settings.h"
#include "../../application/emulator.h"

AudioGui::AudioGui() {
    lang = Lang::getInstance();
    panel = 0;
}

void AudioGui::create() {
    panel = new QWidget;
    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    volume_label = new QLabel("");
    layout->addWidget(volume_label);
    volume = new QHBoxLayout;
    volume->setMargin(0);
    volume_slider = new QSlider(Qt::Horizontal);
    volume_slider->setRange(0, 100);
    volume->addWidget(volume_slider);
    volume_counter_label = new QLabel("");
    volume_counter_label->setFixedWidth(40);
    volume->addWidget(volume_counter_label);
    volume_default = new QPushButton("");
    volume->addWidget(volume_default);
    volume->setSpacing(5);
    layout->addLayout(volume);
    layout->addSpacing(5);

    frequency_label = new QLabel("");
    layout->addWidget(frequency_label);
    frequency = new QHBoxLayout;
    frequency->setMargin(0);
    frequency_slider = new QSlider(Qt::Horizontal);
    frequency_slider->setRange(-300, 300);
    frequency->addWidget(frequency_slider);
    frequency_counter_label = new QLabel("");
    frequency_counter_label->setFixedWidth(40);
    frequency->addWidget(frequency_counter_label);
    frequency_default = new QPushButton("");
    frequency->addWidget(frequency_default);
    frequency->setSpacing(5);
    layout->addLayout(frequency);
    layout->addSpacing(5);

    panel->setLayout(layout);
    panel->setMinimumWidth(400);

    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer);

    translate();

    connect(volume_slider, SIGNAL(valueChanged(int)), this, SLOT(slot_volume_update(int)));
    connect(volume_default, SIGNAL(released()), this, SLOT(slot_volume_default()));
    connect(frequency_slider, SIGNAL(valueChanged(int)), this, SLOT(slot_frequency_update(int)));
    connect(frequency_default, SIGNAL(released()), this, SLOT(slot_frequency_default()));
}

void AudioGui::init() {
    setVolumeDisplay(Settings::get<int>("audio_volume"));
    setFrequencyDisplay(Settings::get<int>("sms_audio_frequency"));
}

void AudioGui::translate() {
    volume_label->setText(lang->get("volume"));
    volume_default->setText(lang->get("default"));
    frequency_label->setText(lang->get("frequency_correction"));
    frequency_default->setText(lang->get("default"));
}

void AudioGui::setVolumeDisplay(int pos) {
    volume_slider->setSliderPosition(pos);
    volume_counter_label->setText(QString::number(pos) + " %");
}

void AudioGui::setFrequencyDisplay(int pos) {
    frequency_slider->setSliderPosition(pos);
    frequency_counter_label->setText(QString::number(pos) + " Hz");
}

//slots
void AudioGui::slot_volume_update(int pos) {
    Settings::set<int>("audio_volume", pos);
    setVolumeDisplay(pos);
    Emulator::getInstance()->adjustVolume(pos);
}

void AudioGui::slot_frequency_update(int pos) {
    Settings::set<int>("sms_audio_frequency", pos );
    setFrequencyDisplay(pos);
    Emulator::getInstance()->updateFrequency();
}

void AudioGui::slot_volume_default() {
    int pos = Settings::setInit("audio_volume");
    slot_volume_update(pos);
}

void AudioGui::slot_frequency_default() {
    int pos = Settings::setInit("sms_audio_frequency");
    slot_frequency_update(pos);
}
