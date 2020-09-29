
#include "cgui.h"
#include "../application/lang.h"
#include "config/audioGui.h"
#include "config/videoGui.h"
#include "config/pathsGui.h"
#include "config/inputGui.h"
#include "config/cheatsGui.h"
#include "config/optionsGui.h"
#include "mgui.h"

CGui::CGui(MGui* mgui)
    : mgui(mgui)
{
    lang = Lang::getInstance();
}

void CGui::create() {
    setWindowFlags(Qt::Window);
    createList();

    panel = new QWidget;
    panel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout = new QHBoxLayout;
    layout->addWidget(list);
    layout->addWidget(panel);
    setLayout(layout);

    createConfigArea();

    translate();
}

void CGui::createList() {
    list = new QListWidget;

    list->addItem(input = new QListWidgetItem(""));
    list->addItem(cheats = new QListWidgetItem(""));
    list->addItem(paths = new QListWidgetItem(""));
    list->addItem(video = new QListWidgetItem(""));
    list->addItem(audio = new QListWidgetItem(""));
    list->addItem(options = new QListWidgetItem(""));

    list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    list->setFixedWidth(100);
    connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(slot_list_update()));
}

void CGui::createConfigArea() {
    pathsGui = new PathsGui();
    pathsGui->create();
    videoGui = new VideoGui();
    videoGui->create();
    audioGui = new AudioGui();
    audioGui->create();
    inputGui = new InputGui();
    inputGui->create();
    cheatsGui = new CheatsGui();
    cheatsGui->create();
    optionsGui = new OptionsGui(this);
    optionsGui->create();

    panelLayout = new QStackedLayout(panel);
    panelLayout->addWidget(pathsGui->get_panel());
    panelLayout->addWidget(videoGui->get_panel());
    panelLayout->addWidget(audioGui->get_panel());
    panelLayout->addWidget(inputGui->get_panel());
    panelLayout->addWidget(cheatsGui->get_panel());
    panelLayout->addWidget(optionsGui->get_panel());
    panel->setLayout(panelLayout);
}

void CGui::_show(Panel panel) {
    switch (panel) {
        case p_input:
            list->setCurrentItem(input);
            break;
        case p_cheats:
            list->setCurrentItem(cheats);
            break;
        case p_paths:
            list->setCurrentItem(paths);
            break;
        case p_video:
            list->setCurrentItem(video);
            break;
        case p_audio:
            list->setCurrentItem(audio);
            break;
        case p_options:
            list->setCurrentItem(options);
            break;
    }
    setWindowFlags(Qt::Window);
    show();
}

void CGui::translate() {
    setWindowTitle(lang->get("config"));
    input->setText(lang->get("input"));
    cheats->setText("Cheats");
    paths->setText(lang->get("paths"));
    video->setText("Video");
    audio->setText("Audio");
    options->setText(lang->get("options"));
}

void CGui::translateAll() {
    translate();
    mgui->translate();
    pathsGui->translate();
    videoGui->translate();
    audioGui->translate();
    optionsGui->translate();
    inputGui->translate();
    cheatsGui->translate();
}

void CGui::initSettings() {
    audioGui->init();
    videoGui->init();
    inputGui->init();
}

void CGui::slot_list_update() {
    QListWidgetItem* item = list->currentItem();

    if      (item == paths) panelLayout->setCurrentWidget(pathsGui->get_panel());
    else if (item == video) panelLayout->setCurrentWidget(videoGui->get_panel());
    else if (item == audio) panelLayout->setCurrentWidget(audioGui->get_panel());
    else if (item == input) panelLayout->setCurrentWidget(inputGui->get_panel());
    else if (item == cheats) panelLayout->setCurrentWidget(cheatsGui->get_panel());
    else if (item == options) panelLayout->setCurrentWidget(optionsGui->get_panel());
}

void CGui::inputPollingComplete() {
    inputGui->pollingComplete();
}

void CGui::cheat_gui_refresh() {
    cheatsGui->refresh();
}

void CGui::cheat_gui_enable_elements(bool state) {
    cheatsGui->enable_elements(state);
}
