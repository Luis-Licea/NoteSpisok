#include "dictionaries.h"
#include "ui_dictionaries.h"

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
