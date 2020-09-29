#include "optionsGui.h"
#include "../../application/lang.h"
#include "../cgui.h"
#include "../../application/main.h"
#include "../../application/settings.h"
#include "../../application/emulator.h"
#include "../application/app.h"

OptionsGui::OptionsGui(CGui* cgui)
    : cgui(cgui)
{
    lang = Lang::getInstance();
    panel = 0;
    vector_pos = 0;
}

void OptionsGui::create() {
    panel = new QWidget;

    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    lang_label = new QLabel();
    layout->addWidget(lang_label);

    lang_list = new QListWidget;
    setupLang();
    connect(lang_list, SIGNAL(currentRowChanged(int)), this, SLOT(slot_lang_update()));

    lang_list->setFixedHeight(50);
    layout->addWidget(lang_list);
    layout->addSpacing(5);

    createSelector();

    border_label = new QLabel("");
    layout->addWidget(border_label);
    border = new QHBoxLayout;
    border->setMargin(0);
    border_switch = new QCheckBox("");
    border->addWidget(border_switch);
    border->setSpacing(5);
    layout->addLayout(border);
    layout->addSpacing(10);

    bios_label = new QLabel("");
    layout->addWidget(bios_label);
    bios = new QHBoxLayout;
    bios->setMargin(0);
    bios_switch = new QCheckBox("");
    bios->addWidget(bios_switch);
    bios->setSpacing(5);
    layout->addLayout(bios);
    layout->addSpacing(10);

    fm_label = new QLabel("");
    layout->addWidget(fm_label);
    fm = new QHBoxLayout;
    fm->setMargin(0);
    fm_switch = new QCheckBox("");
    fm->addWidget(fm_switch);
    fm->setSpacing(5);
    layout->addLayout(fm);
    layout->addSpacing(10);

    glasses_label = new QLabel("");
    layout->addWidget(glasses_label);
    glasses = new QHBoxLayout;
    glasses->setMargin(0);
    glasses_switch = new QCheckBox("");
    glasses->addWidget(glasses_switch);
    glasses->setSpacing(5);
    layout->addLayout(glasses);
    layout->addSpacing(10);

    unit_label = new QLabel("");
    layout->addWidget(unit_label);
    unit = new QHBoxLayout;
    unit->setMargin(0);

    unit_group = new QButtonGroup(panel);

    sms1_radio = new QRadioButton("SMS1");
    unit_group->addButton(sms1_radio);
    unit->addWidget(sms1_radio);

    sms2_radio = new QRadioButton("SMS2");
    unit_group->addButton(sms2_radio);
    unit->addWidget(sms2_radio);

    unit->setSpacing(5);
    layout->addLayout(unit);
    layout->addSpacing(10);

    panel->setLayout(layout);
    panel->setMinimumWidth(400);

    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer);
    translate();

    slot_bios_switch(Settings::get<bool>("sms_use_bios"));
    slot_fm_switch(Settings::get<bool>("sms_use_fm"));
    slot_border_switch(Settings::get<bool>("sms_disable_border"));
    slot_glasses_switch(Settings::get<bool>("sms_disable_3d"));
    int pos = Settings::get<int>("sms_revision");
    sms1_radio->setChecked(pos == 0);
    sms2_radio->setChecked(pos == 1);

    connect(bios_switch, SIGNAL(toggled(bool)), this, SLOT(slot_bios_switch(bool)));
    connect(fm_switch, SIGNAL(toggled(bool)), this, SLOT(slot_fm_switch(bool)));
    connect(border_switch, SIGNAL(toggled(bool)), this, SLOT(slot_border_switch(bool)));
    connect(glasses_switch, SIGNAL(toggled(bool)), this, SLOT(slot_glasses_switch(bool)));
    connect(sms1_radio, SIGNAL(pressed()), this, SLOT(slot_sms1_update()));
    connect(sms2_radio, SIGNAL(pressed()), this, SLOT(slot_sms2_update()));
}

void OptionsGui::translate() {
    lang_label->setText(lang->get("language"));
    bios_label->setText("Bios:");
    fm_label->setText("Fm Sound:");
    border_label->setText(lang->get("border") + ":");
    glasses_label->setText(lang->get("glasses") + ":");
    bios_switch->setText(lang->get("use_bios"));
    fm_switch->setText(lang->get("use_fm"));
    border_switch->setText(lang->get("disable_border"));
    glasses_switch->setText(lang->get("use_glasses"));
    unit_label->setText(lang->get("sms_unit"));
}

void OptionsGui::setupLang() {
    QString lang_path = App::getDir() + _LANG_DIR;

    QDir dir(lang_path);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);
        QString fn = fileInfo.fileName();

        if (fn == "." || fn == "..") continue;
        if (!lang->checkXML(fn)) continue;

        if (vector_pos >= item_list.size()) {
            item_list.resize(item_list.size() + 5);
        }

        lang_list->addItem(item_list[vector_pos++] = new QListWidgetItem(fn));
        if (Settings::get<QString>("language_file") == fn) {
            lang_list->setCurrentItem(item_list[vector_pos-1]);
        }
    }
}

void OptionsGui::createSelector() {
    selector = new QHBoxLayout;
    selector_box = new QComboBox;
    QVector<Emulator::Models> _models = Emulator::getModels();
    for (int i = 0; i < _models.size(); i++) {
        if (!_models[i].inUse) continue;
        selector_box->addItem(_models[i].desc);
    }
    selector_box->setFixedWidth(80);
    selector->setAlignment(Qt::AlignRight);
    selector->addWidget(selector_box);
    connect(selector_box, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_setSelector()));
    layout->addLayout(selector);
}

//slots
void OptionsGui::slot_lang_update() {
    QListWidgetItem* item = lang_list->currentItem();

    for (int z = 0; z < vector_pos; z++) {
        if (item_list[z] == item) {
            Settings::set<QString>("language_file", lang_list->item(z)->text());
            lang->setFile(lang_list->item(z)->text());
            cgui->translateAll();
            break;
        }
    }
}

void OptionsGui::slot_setSelector() {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();

    unit_label->setHidden(true);
    sms1_radio->setHidden(true);
    sms2_radio->setHidden(true);
    bios_label->setHidden(true);
    bios_switch->setHidden(true);
    fm_label->setHidden(true);
    fm_switch->setHidden(true);
    border_label->setHidden(true);
    border_switch->setHidden(true);
    glasses_label->setHidden(true);
    glasses_switch->setHidden(true);

    if (_models[id].ident == "sms") {
        unit_label->setHidden(false);
        sms1_radio->setHidden(false);
        sms2_radio->setHidden(false);
        bios_label->setHidden(false);
        bios_switch->setHidden(false);
        fm_label->setHidden(false);
        fm_switch->setHidden(false);
        border_label->setHidden(false);
        border_switch->setHidden(false);
        glasses_label->setHidden(false);
        glasses_switch->setHidden(false);
        slot_bios_switch(Settings::get<bool>("sms_use_bios"));
        slot_border_switch(Settings::get<bool>("sms_disable_border"));
    } else if (_models[id].ident == "gg") {
        bios_label->setHidden(false);
        bios_switch->setHidden(false);
        slot_bios_switch(Settings::get<bool>("gg_use_bios"));
    } else if (_models[id].ident == "sg") {
        border_label->setHidden(false);
        border_switch->setHidden(false);
        slot_border_switch(Settings::get<bool>("sg_disable_border"));
    }
}

void OptionsGui::slot_bios_switch(bool state) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    Settings::set<bool>(_models[id].ident + "_use_bios", state);
    bios_switch->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

void OptionsGui::slot_fm_switch(bool state) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    Settings::set<bool>(_models[id].ident + "_use_fm", state);
    fm_switch->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

void OptionsGui::slot_border_switch(bool state) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    Settings::set<bool>(_models[id].ident + "_disable_border", state);
    border_switch->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

void OptionsGui::slot_glasses_switch(bool state) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    Settings::set<bool>(_models[id].ident + "_disable_3d", state);
    glasses_switch->setCheckState(state ? Qt::Checked : Qt::Unchecked);
}

void OptionsGui::slot_sms1_update() {
    Settings::set<int>("sms_revision", 0);
}

void OptionsGui::slot_sms2_update() {
    Settings::set<int>("sms_revision", 1);
}


