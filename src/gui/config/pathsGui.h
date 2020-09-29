#ifndef PATHSGUI_H
#define PATHSGUI_H

#include <QtGui/QtGui>
class Lang;

class PathsGui : public QObject {
    Q_OBJECT

    Lang* lang;
    QWidget* panel;

    QVBoxLayout* layout;

    QHBoxLayout* selector;
    QComboBox* selector_box;

    QLabel* roms_label;
    QHBoxLayout* roms;
    QLineEdit* roms_path;
    QPushButton* roms_select;
    QPushButton* roms_default;

    QLabel* sram_label;
    QHBoxLayout* sram;
    QLineEdit* sram_path;
    QPushButton* sram_select;
    QPushButton* sram_default;

    QLabel* state_label;
    QHBoxLayout* state;
    QLineEdit* state_path;
    QPushButton* state_select;
    QPushButton* state_default;

    QLabel* cheat_label;
    QHBoxLayout* cheat;
    QLineEdit* cheat_path;
    QPushButton* cheat_select;
    QPushButton* cheat_default;

    QWidget* jp_bios_widget;
    QVBoxLayout* jp_bios_box;
    QLabel* jp_bios_label;
    QHBoxLayout* jp_bios;
    QLineEdit* jp_bios_path;
    QPushButton* jp_bios_select;
    QPushButton* jp_bios_default;

    QWidget* us_bios_widget;
    QVBoxLayout* us_bios_box;
    QLabel* us_bios_label;
    QHBoxLayout* us_bios;
    QLineEdit* us_bios_path;
    QPushButton* us_bios_select;
    QPushButton* us_bios_default;

    void createSelector();
    QString set_folder(QString title, QString cur_path);
    QString set_file(QString title, QString cur_path);
    void slot_path_default(QString theme);
    void slot_path_select(QString theme);
    void slot_bios_default(QString theme);
    void slot_bios_select(QString theme);

public:
    PathsGui();
    void create();
    void translate();
    QWidget* get_panel() {
        return panel;
    }

public slots:
    void setSelector();
    void slot_roms_select() { slot_path_select("rom"); }
    void slot_roms_default() { slot_path_default("rom"); }
    void slot_sram_select() { slot_path_select("sram"); }
    void slot_sram_default() { slot_path_default("sram"); }
    void slot_cheat_select() { slot_path_select("cheat"); }
    void slot_cheat_default() { slot_path_default("cheat"); }
    void slot_state_select() { slot_path_select("savestate"); }
    void slot_state_default() { slot_path_default("savestate"); }
    void slot_jp_bios_select() { slot_bios_select("jp"); }
    void slot_jp_bios_default() { slot_bios_default("jp"); }
    void slot_us_bios_select() { slot_bios_select("us"); }
    void slot_us_bios_default() { slot_bios_default("us"); }
};

#endif // PATHSGUI_H
