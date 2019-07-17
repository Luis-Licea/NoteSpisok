#include "rename.h"
#include "ui_rename.h"

Rename::Rename(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rename)
{
    ui->setupUi(this);
}

Rename::~Rename()
{
    delete ui;
}

/**
 * @brief Rename::on_buttonBox_accepted
 * Relays the text entered in the line edit box.
 */
void Rename::on_buttonBox_accepted()
{
    emit relayName(ui->lineEditRename->text());
}
