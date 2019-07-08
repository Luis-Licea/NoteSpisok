#include "dictionaries.h"
#include "ui_dictionaries.h"

Dictionaries::Dictionaries(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dictionaries)
{
    ui->setupUi(this);
}

Dictionaries::~Dictionaries()
{
    delete ui;
}

