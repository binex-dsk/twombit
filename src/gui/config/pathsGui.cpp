#include "pathsGui.h"
#include "../../application/lang.h"
#include "../../application/settings.h"
#include "../../application/emulator.h"
#include "../application/app.h"

PathsGui::PathsGui() {
    lang = Lang::getInstance();
    panel = 0;
}

void PathsGui::create() {
    panel = new QWidget;

    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    createSelector();

    roms_label = new QLabel("");
    layout->addWidget(roms_label);
    roms = new QHBoxLayout;
    roms->setMargin(0);
    roms_path = new QLineEdit;
    roms_path->setReadOnly(true);
    roms->addWidget(roms_path);
    roms_select = new QPushButton("");
    roms->addWidget(roms_select);
    roms_default = new QPushButton("");
    roms->addWidget(roms_default);
    roms->setSpacing(5);
    layout->addLayout(roms);
    layout->addSpacing(5);

    sram_label = new QLabel("");
    layout->addWidget(sram_label);
    sram = new QHBoxLayout;
    sram->setMargin(0);
    sram_path = new QLineEdit;
    sram_path->setReadOnly(true);
    sram->addWidget(sram_path);
    sram_select = new QPushButton("");
    sram->addWidget(sram_select);
    sram_default = new QPushButton("");
    sram->addWidget(sram_default);
    sram->setSpacing(5);
    layout->addLayout(sram);
    layout->addSpacing(5);

    state_label = new QLabel("");
    layout->addWidget(state_label);
    state = new QHBoxLayout;
    state->setMargin(0);
    state_path = new QLineEdit;
    state_path->setReadOnly(true);
    state->addWidget(state_path);
    state_select = new QPushButton("");
    state->addWidget(state_select);
    state_default = new QPushButton("");
    state->addWidget(state_default);
    state->setSpacing(5);
    layout->addLayout(state);
    layout->addSpacing(5);

    cheat_label = new QLabel("");
    layout->addWidget(cheat_label);
    cheat = new QHBoxLayout;
    cheat->setMargin(0);
    cheat_path = new QLineEdit;
    cheat_path->setReadOnly(true);
    cheat->addWidget(cheat_path);
    cheat_select = new QPushButton("");
    cheat->addWidget(cheat_select);
    cheat_default = new QPushButton("");
    cheat->addWidget(cheat_default);
    cheat->setSpacing(5);
    layout->addLayout(cheat);
    layout->addSpacing(5);

    jp_bios_widget = new QWidget;
    jp_bios_box = new QVBoxLayout;
    jp_bios_box->setMargin(0);
    jp_bios_box->setSpacing(0);
    jp_bios_label = new QLabel("");
    jp_bios_box->addWidget(jp_bios_label);
    jp_bios = new QHBoxLayout;
    jp_bios->setMargin(0);
    jp_bios_path = new QLineEdit;
    jp_bios_path->setReadOnly(true);
    jp_bios->addWidget(jp_bios_path);
    jp_bios_select = new QPushButton("");
    jp_bios->addWidget(jp_bios_select);
    jp_bios_default = new QPushButton("");
    jp_bios->addWidget(jp_bios_default);
    jp_bios->setSpacing(5);
    jp_bios_box->addLayout(jp_bios);
    jp_bios_widget->setLayout(jp_bios_box);
    layout->addWidget(jp_bios_widget);
    layout->addSpacing(5);

    us_bios_widget = new QWidget;
    us_bios_box = new QVBoxLayout;
    us_bios_box->setMargin(0);
    us_bios_box->setSpacing(0);
    us_bios_label = new QLabel("");
    us_bios_box->addWidget(us_bios_label);
    us_bios = new QHBoxLayout;
    us_bios->setMargin(0);
    us_bios_path = new QLineEdit;
    us_bios_path->setReadOnly(true);
    us_bios->addWidget(us_bios_path);
    us_bios_select = new QPushButton("");
    us_bios->addWidget(us_bios_select);
    us_bios_default = new QPushButton("");
    us_bios->addWidget(us_bios_default);
    us_bios->setSpacing(5);
    us_bios_box->addLayout(us_bios);
    us_bios_widget->setLayout(us_bios_box);
    layout->addWidget(us_bios_widget);
    layout->addSpacing(5);

    panel->setLayout(layout);
    panel->setMinimumWidth(400);

    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer);
    translate();

    connect(roms_select, SIGNAL(released()), this, SLOT(slot_roms_select()));
    connect(sram_select, SIGNAL(released()), this, SLOT(slot_sram_select()));
    connect(cheat_select, SIGNAL(released()), this, SLOT(slot_cheat_select()));
    connect(state_select, SIGNAL(released()), this, SLOT(slot_state_select()));
    connect(jp_bios_select, SIGNAL(released()), this, SLOT(slot_jp_bios_select()));
    connect(us_bios_select, SIGNAL(released()), this, SLOT(slot_us_bios_select()));

    connect(roms_default, SIGNAL(released()), this, SLOT(slot_roms_default()));
    connect(sram_default, SIGNAL(released()), this, SLOT(slot_sram_default()));
    connect(cheat_default, SIGNAL(released()), this, SLOT(slot_cheat_default()));
    connect(state_default, SIGNAL(released()), this, SLOT(slot_state_default()));
    connect(jp_bios_default, SIGNAL(released()), this, SLOT(slot_jp_bios_default()));
    connect(us_bios_default, SIGNAL(released()), this, SLOT(slot_us_bios_default()));
}

void PathsGui::createSelector() {
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
    connect(selector_box, SIGNAL(currentIndexChanged(int)), this, SLOT(setSelector()));
    layout->addLayout(selector);
}

void PathsGui::translate() {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();

    roms_label->setText(lang->get("rom_path"));
    QString path = Settings::get<QString>(_models[id].ident + "_path_rom");
    roms_path->setText((path=="") ? "" : path);
    roms_select->setText(lang->get("select"));
    roms_default->setText(lang->get("default"));

    sram_label->setText(lang->get("sram_path"));
    path = Settings::get<QString>(_models[id].ident + "_path_sram");
    sram_path->setText((path=="") ? "app/sram" : path);
    sram_select->setText(lang->get("select"));
    sram_default->setText(lang->get("default"));

    state_label->setText(lang->get("state_path"));
    path = Settings::get<QString>(_models[id].ident + "_path_savestate");
    state_path->setText((path=="") ? "app/savestate" : path);
    state_select->setText(lang->get("select"));
    state_default->setText(lang->get("default"));

    cheat_label->setText(lang->get("cheat_path"));
    path = Settings::get<QString>(_models[id].ident + "_path_cheat");
    cheat_path->setText((path=="") ? "app/cheat" : path);
    cheat_select->setText(lang->get("select"));
    cheat_default->setText(lang->get("default"));

    path = "";
    jp_bios_label->setText(lang->get("bios_jp"));
    if (_models[id].ident == "gg" || _models[id].ident == "sms") {
        path = Settings::get<QString>(_models[id].ident + "_path_jp_bios");
    }
    jp_bios_path->setText(path);
    jp_bios_select->setText(lang->get("select"));
    jp_bios_default->setText(lang->get("default"));

    path = "";
    us_bios_label->setText(lang->get("bios_us"));
    if (_models[id].ident == "sms") {
        path = Settings::get<QString>(_models[id].ident + "_path_us_bios");
    }
    us_bios_path->setText(path);
    us_bios_select->setText(lang->get("select"));
    us_bios_default->setText(lang->get("default"));
}

QString PathsGui::set_folder(QString title, QString cur_path) {
    return QFileDialog::getExistingDirectory(0, title, cur_path == "" ? App::getDir() : cur_path, QFileDialog::ShowDirsOnly);
}

QString PathsGui::set_file(QString title, QString cur_path) {
   return QFileDialog::getOpenFileName(0,
      title, cur_path == "" ? App::getDir() : cur_path, "*.*");
}

void PathsGui::slot_path_select(QString theme) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    QString curFolder = Settings::get<QString>(_models[id].ident + "_path_" + theme);

    QString folder = set_folder(lang->get(theme + "_path"), curFolder);
    if (folder != "") {
        Settings::set<QString>(_models[id].ident + "_path_" + theme, folder);
        translate();
    }
}

void PathsGui::slot_path_default(QString theme) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    Settings::set<QString>(_models[id].ident + "_path_" + theme, "");
    translate();
}

void PathsGui::slot_bios_default(QString theme) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    Settings::set<QString>(_models[id].ident + "_path_" + theme + "_bios", "");
    translate();
}

void PathsGui::slot_bios_select(QString theme) {
    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    QString curFile = Settings::get<QString>(_models[id].ident + "_path_" + theme + "_bios");
    QString newFile = set_file(lang->get("bios_" + theme), curFile);
    if (newFile == "") return;
    Settings::set<QString>(_models[id].ident + "_path_" + theme + "_bios", newFile);
    translate();
}

//slots
void PathsGui::setSelector() {
    us_bios_widget->setHidden(false);
    jp_bios_widget->setHidden(false);

    int id = selector_box->currentIndex();
    QVector<Emulator::Models> _models = Emulator::getModels();
    if (_models[id].ident == "sg") {
        us_bios_widget->setHidden(true);
        jp_bios_widget->setHidden(true);
    } else if (_models[id].ident == "gg") {
        us_bios_widget->setHidden(true);
    }
    translate();
}
