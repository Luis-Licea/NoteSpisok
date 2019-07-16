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

void Delete::deleteTerm(QString termToDelete)
{
    ui->label->setText("Delete " + termToDelete + "?");
}
