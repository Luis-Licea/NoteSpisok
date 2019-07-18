#include "delete.h"
#include "ui_delete.h"
#include <QFile>
#include "mainwindow.h"

Delete::Delete(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Delete)
{
    ui->setupUi(this);  
}

Delete::~Delete()
{
    delete ui;
}

void Delete::showDeleteWarning(QString term)
{
    ui->label->setText("Delete " + term + "?");
}
