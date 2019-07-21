#include "dictionaries.h"
#include "ui_dictionaries.h"
#include "mainwindow.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

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

/**
 * @brief Dictionaries::on_pushButtonDelete_clicked
 * Launches a dialog window for deleting a dictionary.
 */
void Dictionaries::on_pushButtonDelete_clicked()
{
    //Create a warning window
    mDelete = new Delete{this};
    mDelete->setWindowTitle("Delete");

    //Connect the windows to display the dictionary to be deleted
    QObject::connect(this, SIGNAL(relayDictionary(QString)), mDelete, SLOT(showDeleteWarning(QString)));
    //Connect the windows to delete the dicitonary when the user accepts
    QObject::connect(mDelete, SIGNAL(accepted()), SLOT(deleteDictionary()));
    //Emit the signal to create the warning
    emit relayDictionary(ui->listWidget->currentItem()->text());
    //Display the warning
    mDelete->show();
}

/**
 * @brief Dictionaries::deleteDictionary
 * Deletes the selected dictionary and all of its contents.
 */
void Dictionaries::deleteDictionary()
{
    //Check that an item is selected before proceeding
    if (ui->listWidget->isItemSelected(ui->listWidget->currentItem()))
    {
        QString folderToDelete{ui->listWidget->currentItem()->text()};

        QDir folderPath{resourcesFolder + folderToDelete};

        if (folderPath.exists())
            //Go to the parent folder and delete the folder
            folderPath.removeRecursively();
        //Clear the list widget and reload it
        ui->listWidget->clear();
        loadTermFolders();
        emit signalLoadTermFolders();
    }
}

/**
 * @brief Dictionaries::on_pushButtonRename_clicked
 * Launches a dialog window for renaming.
 */
void Dictionaries::on_pushButtonRename_clicked()
{
    //Check that an item is slected before attempting to
    //rename its corresponging folder
    if (ui->listWidget->isItemSelected(ui->listWidget->currentItem()))
    {
        mRename = new Rename{this};
        mRename->setWindowTitle("Rename");
        mRename->show();
        QObject::connect(mRename, SIGNAL(relayName(QString)), this, SLOT(renameDictionary(QString)));
    }
}

/**
 * @brief Dictionaries::renameDictionary
 * Renames a dictionary's folder without removing its contents.
 * @param newDictionaryName the new dictionary name that will
 * replace the previos one without touching the dictionary's
 * files.
 */
void Dictionaries::renameDictionary(QString const &newName)
{
    QString currentName{ui->listWidget->currentItem()->text()};

    QDir parentDirectory;

    parentDirectory.cd(resourcesFolder);
    parentDirectory.rename(currentName, newName);

    //Clear the list widget and reload it
    ui->listWidget->clear();
    loadTermFolders();
    emit signalLoadTermFolders();
}
