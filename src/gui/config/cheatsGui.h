#ifndef CHEATSGUI_H
#define CHEATSGUI_H

#include <QtGui/QtGui>
class Lang;

class CheatsGui : public QObject {
    Q_OBJECT

    Lang* lang;
    QWidget* panel;

    QVBoxLayout *layout;
    QHBoxLayout *input_line_code;
    QHBoxLayout *input_line_desc;

    QLabel *code_label;
    QLineEdit *code_edit;
    QPushButton *new_button;
    QPushButton *del_button;

    QLabel *desc_label;
    QLineEdit *desc_edit;
    QPushButton *active_button;
    QPushButton *edit_button;

    QTreeWidgetItem* items[100];

    QTreeWidget *list;

public:
    CheatsGui();
    void create();
    QWidget* get_panel() {
        return panel;
    }
    void enable_elements(bool state);
    void refresh();
    void translate();

public slots:
    void new_code();
    void del_code();
    void edit_code();
    void active_code();
    void click_item();
};

#endif //CHEATSGUI_H
