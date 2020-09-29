#include "cheatsGui.h"
#include "../../application/lang.h"
#include "../../application/cheatshandler.h"
#include "../../application/dialog.h"

CheatsGui::CheatsGui() {
    lang = Lang::getInstance();
    panel = 0;
}

void CheatsGui::create() {
    panel = new QWidget;

    layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    code_label = new QLabel( "" );
    layout->addWidget(code_label);

    input_line_code = new QHBoxLayout;
    input_line_code->setMargin(0);
    code_edit = new QLineEdit;
    input_line_code->addWidget(code_edit);
    input_line_code->setSpacing(10);
    new_button = new QPushButton("");
    input_line_code->addWidget(new_button);
    input_line_code->setSpacing(10);
    del_button = new QPushButton("");
    input_line_code->addWidget(del_button);
    layout->addLayout(input_line_code);
    layout->addSpacing(10);

    desc_label = new QLabel("");
    layout->addWidget(desc_label);

    input_line_desc = new QHBoxLayout;
    input_line_desc->setMargin(0);
    desc_edit = new QLineEdit;
    input_line_desc->addWidget(desc_edit);
    input_line_desc->setSpacing(10);
    active_button = new QPushButton("");
    input_line_desc->addWidget(active_button);
    input_line_desc->setSpacing(10);
    edit_button = new QPushButton("");
    input_line_desc->addWidget(edit_button);
    layout->addLayout(input_line_desc);
    layout->addSpacing(30);

    list = new QTreeWidget;
    list->setColumnCount(3);
    list->setColumnWidth(0, 100);
    list->setColumnWidth(1, 150);
    layout->addWidget(list);
    layout->addSpacing(15);

    panel->setLayout(layout);
    panel->setMinimumWidth(400);

    QWidget *spacer2 = new QWidget;
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(spacer2);

    translate();

    connect(new_button, SIGNAL(released()), this, SLOT(new_code()));
    connect(del_button, SIGNAL(released()), this, SLOT(del_code()));
    connect(edit_button, SIGNAL(released()), this, SLOT(edit_code()));
    connect(active_button, SIGNAL(released()), this, SLOT(active_code()));

    enable_elements(false);
}

void CheatsGui::translate() {
    new_button->setText(lang->get("cheat_add_code"));
    del_button->setText(lang->get("cheat_delete_code"));
    code_label->setText(lang->get("cheat_code"));
    desc_label->setText(lang->get("cheat_description"));
    active_button->setText(lang->get("cheat_active_code"));
    edit_button->setText(lang->get("cheat_edit_code"));
    list->setHeaderLabels(QStringList() << lang->get("cheat_active") << lang->get("cheat_code") << lang->get("cheat_description"));
    refresh();
}

void CheatsGui::enable_elements(bool state) {
    new_button->setEnabled( state );
    del_button->setEnabled( state );
    edit_button->setEnabled( state );
    active_button->setEnabled( state );

    code_edit->setText("");
    desc_edit->setText("");

    code_edit->setEnabled( state );
    desc_edit->setEnabled( state );
    list->setEnabled( state );
}

void CheatsGui::refresh() {
    list->clear();
    unsigned count = CheatsHandler::getInstance()->count();

    for(unsigned i=0; i<count; i++) {
        CheatsHandler::Code code = CheatsHandler::getInstance()->get(i);
        QTreeWidgetItem *item = new QTreeWidgetItem(list);
        item->setText(0, code.enabled ? lang->get("yes") : lang->get("no"));
        item->setText(1, code.cheat);
        item->setText(2, code.desc);
        items[i] = item;
    }
    connect(list, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(click_item()));
    connect(list, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(active_code()));
}

//slots
void CheatsGui::new_code() {
    if (CheatsHandler::getInstance()->count() >= 100) {
        Dialog::Warn(Lang::getInstance()->get("cheat_limit"));
        return;
    }

    if (!CheatsHandler::getInstance()->add(code_edit->text(), desc_edit->text())) {
        Dialog::Warn(Lang::getInstance()->get("cheat_invalid"));
        return;
    }
    code_edit->setText("");
    desc_edit->setText("");
    refresh();
}

void CheatsGui::del_code() {
    unsigned count = CheatsHandler::getInstance()->count();
    QTreeWidgetItem *item = list->currentItem();
    for (unsigned i=0; i<count; i++) {
        if(items[i] == item) {
            CheatsHandler::getInstance()->remove(i);
            refresh();
            return;
        }
    }
    Dialog::Warn(Lang::getInstance()->get("cheat_select"));
}

void CheatsGui::edit_code() {
    unsigned count = CheatsHandler::getInstance()->count();
    QTreeWidgetItem *item = list->currentItem();
    for (unsigned i=0; i<count; i++) {
        if(items[i] == item) {
            if (CheatsHandler::getInstance()->edit(code_edit->text(), desc_edit->text(), i)) {
                refresh();
                return;
            }
            Dialog::Warn(Lang::getInstance()->get("cheat_invalid"));
            return;
        }
    }
    Dialog::Warn(Lang::getInstance()->get("cheat_select"));
}

void CheatsGui::active_code() {
    unsigned count = CheatsHandler::getInstance()->count();
    QTreeWidgetItem *item = list->currentItem();
    for (unsigned i=0; i<count; i++) {
        if(items[i] == item) {
            CheatsHandler::getInstance()->toggle(i);
            refresh();
            return;
        }
    }
}

void CheatsGui::click_item() {
    unsigned count = CheatsHandler::getInstance()->count();
    QTreeWidgetItem *item = list->currentItem();
    for (unsigned i=0; i<count; i++) {
        if(items[i] == item) {
            code_edit->setText(item->text(1));
            desc_edit->setText(item->text(2));
            return;
        }
    }
}
