#include "inputGui.h"
#include "../../application/lang.h"
#include "../../application/emulator.h"
#include "../../application/dialog.h"
#include "../../application/settings.h"

InputGui::InputGui() {
    lang = Lang::getInstance();
    panel = 0;
}

void InputGui::create() {
    panel = new QWidget;
    QWidget* spacer = new QWidget;

    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    control = new QHBoxLayout;
        device = new QComboBox;
        control->addWidget(device);
        control->setSpacing(10);
        label = new QLabel("-->");
        control->addWidget(label);
        control->setSpacing(10);
        empty = new QPushButton("");
        control->addWidget(empty);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        control->addWidget(spacer);

    layout->addLayout(control);
    layout->addSpacing(10);

    list = new QTreeWidget;
    list->setColumnCount(3);

    list->setColumnWidth(0, 100);
    list->setColumnWidth(1, 150);
    layout->addWidget(list);
    layout->addSpacing(10);

    info = new QLabel("");
    layout->addWidget(info);
    layout->addSpacing(10);

    control2 = new QHBoxLayout;
        labelPort1 = new QLabel("Port 1:");
        control2->addWidget(labelPort1);
        control2->setSpacing(10);
        devicePort1 = new QComboBox;
        control2->addWidget(devicePort1);

        labelPort2 = new QLabel("Port 2:");
        control2->addWidget(labelPort2);
        control2->setSpacing(10);
        devicePort2 = new QComboBox;
        control2->addWidget(devicePort2);
        spacer = new QWidget;
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        control2->addWidget(spacer);

    layout->addLayout(control2);
    panel->setLayout(layout);
    panel->setMinimumWidth(400);
    spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer);

    setPortCombo(devicePort1, 0);
    setPortCombo(devicePort2, 1);
    setDevices();

    translate(true);
    connect(empty, SIGNAL(released()), this, SLOT(slot_empty_device()));
}

void InputGui::translate(bool first) {
    empty->setText(lang->get("empty"));
    list->setHeaderLabels(QStringList() << "Input" << lang->get("device") << lang->get("map"));
    info->setText(lang->get("register_input"));
    if(!first) slot_set_device_map(device->currentIndex());
}

void InputGui::init() {
    slot_set_device_map(device->currentIndex());
}

void InputGui::setDevices() {
    unsigned count = Emulator::getInstance()->getDeviceCount();
    for(int i=0; i < count; i++) {
        QString entry = Emulator::getInstance()->getDevicePort(i)
                        + Emulator::getInstance()->getDeviceName(i);
        device->addItem( entry );
    }

    connect(device, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_set_device_map(int)));
}

void InputGui::pollingComplete() {
    slot_set_device_map(device->currentIndex());
    info->setText(lang->get("register_input"));
}

void InputGui::setPortCombo(QComboBox* box, unsigned port) {
    int deviceId = Settings::get<int>("sms_port_" + QString( port == 0 ? "1" : "2" ) );
    box->clear();
    QVector<Emulator::Devices> devList = Emulator::getInstance()->getDevicesForPort(port);
    box->addItem(lang->get("none"), QVariant(0));
    box->setCurrentIndex(0);
    for(int i=0; i < devList.size(); i++) {
        box->addItem(devList[i].name, QVariant(devList[i].id));
        if (deviceId == devList[i].id) {
            box->setCurrentIndex(i+1);
        }
    }
    connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_change_port(int)));
}

//slots
void InputGui::slot_empty_device() {
    if (Dialog::Confirm (lang->get("clear_port"), lang->get("ok"), lang->get("cancel"))) {
        Emulator::getInstance()->emptyDevice(device->currentIndex());
        slot_set_device_map(device->currentIndex());
        info->setText(lang->get("register_input"));
    }
}

void InputGui::slot_set_device_map(int id) {
    list->clear();
    unsigned count = Emulator::getInstance()->getDeviceObjectCount(id);
    for (int i=0; i < count; i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(list);
        QString content = Emulator::getInstance()->getDeviceObjectEntry(id, i);
        QStringList content_list = content.split("|");
        item->setText(0, lang->get(content_list[0]));
        item->setText(1, content_list[1]);
        item->setText(2, content_list[2]);
        items[i] = item;
    }
    connect(list, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slot_map_input()));
}

void InputGui::slot_map_input() {
    info->setText( Emulator::getInstance()->getUpdateString(device->currentIndex(), list->currentIndex().row()) );
    Emulator::getInstance()->captureReady(device->currentIndex(), list->currentIndex().row());
}

void InputGui::slot_change_port(int index) {
    QString port = "1";
    if (QObject::sender() == devicePort2) {
        port = "2";
    }
    int deviceID = (qobject_cast<QComboBox*>( QObject::sender() ))->itemData(index).toInt();
    Settings::set<int>("sms_port_" + port, deviceID);
    Emulator::getInstance()->updatePort();
}
