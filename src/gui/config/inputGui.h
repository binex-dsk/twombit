#ifndef INPUTGUI_H
#define INPUTGUI_H

#include <QtGui/QtGui>
class Lang;

class InputGui : public QObject {
    Q_OBJECT
    Lang* lang;
    QWidget* panel;

    QVBoxLayout* layout;
    QHBoxLayout* control;
    QComboBox* device;
    QPushButton* empty;
    QLabel* label;
    QTreeWidget* list;
    QTreeWidgetItem* items[12];
    QLabel* info;

    QHBoxLayout* control2;
    QLabel* labelPort1;
    QComboBox* devicePort1;
    QLabel* labelPort2;
    QComboBox* devicePort2;

    void setDevices();
    void setPortCombo(QComboBox* box, unsigned port);

public:
    InputGui();
    void create();
    void translate(bool first = false);
    void init();
    void pollingComplete();
    QWidget* get_panel() {
        return panel;
    }

public slots:
    void slot_empty_device();
    void slot_set_device_map(int id);
    void slot_map_input();
    void slot_change_port(int index);
};

#endif //INPUTGUI_H
