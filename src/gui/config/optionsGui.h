#ifndef OPTIONSGUI_H
#define OPTIONSGUI_H

#include <QtGui/QtGui>
class Lang;
class CGui;

class OptionsGui : public QObject {
    Q_OBJECT

    CGui* cgui;
    Lang* lang;

    QWidget* panel;
    QVBoxLayout* layout;
    QHBoxLayout* selector;
    QComboBox* selector_box;

    QLabel* lang_label;
    QListWidget* lang_list;

    QHBoxLayout* bios;
    QLabel* bios_label;
    QCheckBox* bios_switch;

    QHBoxLayout* fm;
    QLabel* fm_label;
    QCheckBox* fm_switch;

    QHBoxLayout* border;
    QLabel* border_label;
    QCheckBox* border_switch;

    QHBoxLayout* glasses;
    QLabel* glasses_label;
    QCheckBox* glasses_switch;

    QHBoxLayout* unit;
    QLabel* unit_label;
    QButtonGroup* unit_group;
    QRadioButton* sms1_radio;
    QRadioButton* sms2_radio;

    QVector<QListWidgetItem*> item_list;
    int vector_pos;
    void setupLang();
    void createSelector();

public:
    OptionsGui(CGui* cgui);
    void create();
    QWidget* get_panel() {
        return panel;
    }
    void translate();

public slots:
    void slot_lang_update();
    void slot_setSelector();
    void slot_bios_switch(bool state);
    void slot_fm_switch(bool state);
    void slot_border_switch(bool state);
    void slot_glasses_switch(bool state);
    void slot_sms1_update();
    void slot_sms2_update();
};

#endif //OPTIONSGUI_H
