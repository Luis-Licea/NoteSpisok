#include "dictionaries.h"
#include "ui_dictionaries.h"
#include "mainwindow.h"

#include <QDir>
#include <QFileInfo>

QString const resourcesFolder{"resources/"};
QString const historyFile{"resources/history.txt"};
QString const temporaryFile{"resources/temp.txt"};

Dictionaries::Dictionaries(QWidget *parent) :
    QDialog{parent},
    ui{new Ui::Dictionaries}
{
    ui->setupUi(this);

    loadTermFolders();
}

Dictionaries::~Dictionaries()
{
    delete ui;
}

/**
 * @brief Dictionaries::loadTermFolders
 * Loads the term folders so that folders can
 * be deleted or renamed.
 */
void Dictionaries::loadTermFolders()
{
    //Open the resources folder
    //Add the folders to the list widget
    QDir dictionaries{resourcesFolder};
    for (QFileInfo item: dictionaries.entryInfoList())
        if (item.isDir() && item.baseName() != "")
            ui->listWidget->addItem(item.baseName());
}

/**
 * @brief Dictionaries::on_pushButtonAdd_clicked
 * Adds a new term folder to the resources folder
 * and sends a signal to the main window to
 * update the term folders.
 */
void Dictionaries::on_pushButtonAdd_clicked()
{
    QString newFolderName{ui->lineEdit->text()};

    QDir newTermFolder{resourcesFolder + newFolderName};
    if (!newTermFolder.exists())
        //Go to the parent folder and create the folder
        newTermFolder.mkdir("../" + newFolderName);

    //Clear the list widget and reload it
    ui->listWidget->clear();
    loadTermFolders();
    emit signalLoadTermFolders();
}
