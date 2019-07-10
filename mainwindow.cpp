#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QString>
#include <QByteArray>
#include <QDir>
#include <QDebug>
#include <QList>

QString const resourcesFolder{"resources/"};
QString const historyFile{"resources/history.txt"};

/**
 * @brief MainWindow::currentTermFolder
 * Returns the current folder where terms are being saved.
 * @return the current folder where terms are being saved
 */
QString MainWindow::currentTermFolder() const
{
    //Obtain the current folder name from the combo box name
    QString currentTermFolder{ui->comboBoxDictionaries->currentText()};
    return resourcesFolder + currentTermFolder + "/";
}

/**
 * @brief MainWindow::loadTermFolders
 * Loads the term folders containing the term definitions.
 */
void MainWindow::loadTermFolders()
{
    QDir const dir{resourcesFolder};

    //If resourcesFolder does not exist, go to the parent directory and create it
    if (!dir.exists())
        dir.mkdir("../" + resourcesFolder);

    //Add every term directory in the resourcesFolder to the combo box
    for(QFileInfo item: dir.entryInfoList())
    {
        if (item.isDir() && item.baseName() != "")
            ui->comboBoxDictionaries->addItem(item.baseName());
            //Only use qPrintable for debugging
            //qPrintable(item.baseName()) causes errors displaying cyrillic
    }
}

/**
 * @brief MainWindow::loadTerms
 * Loads the terms that are inside the specified term folder.
 */
void MainWindow::loadTerms()
{
    QDir dir{resourcesFolder};

    //Change directory to the term folder specified by the combo box
    QString const comboBoxDictionariesContents{
        ui->comboBoxDictionaries->currentText()};
    dir.cd(comboBoxDictionariesContents);

    //Disable the Delete and Save buttons because no terms are selected
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonSave->setEnabled(false);

    //Create a termList that will be used in the string completer
    QStringList termList;

    //Add every file inside the term folder into the widget list
    //Add every filename to the termList
    for(QFileInfo item: dir.entryInfoList())
    {
        if (item.isFile())
        {
            ui->listWidgetEntries->addItem(item.fileName());
            termList << item.fileName();
            //Only use qPrintable for debugging
            //qPrintable(item.baseName()) causes errors displaying cyrillic
        }
    }

    //Create a new string completer using the termList
    //Disable case sensitivity and set the completer
    mStringCompleter = new QCompleter(termList, this);
    mStringCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditSearch->setCompleter(mStringCompleter);
}

/**
 * @brief MainWindow::MainWindow
 * Creates the main window and loads the term folders.
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent}, ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    loadTermFolders();

    //Disable the Delete and Save buttons because no terms are selected
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonSave->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionDictionaries_triggered()
{
    mDictionaries = new Dictionaries{this};
    mDictionaries->show();
}

void MainWindow::on_actionConfiguration_triggered()
{
    mConfiguration = new Configuration{this};
    mConfiguration->show();
}

void MainWindow::on_actionAboutApp_triggered()
{
    mAboutApp = new AboutApp{this};
    mAboutApp->show();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

/**
 * @brief MainWindow::on_pushButtonSave_clicked
 * Saves the edit-box contents into the file
 * of the currently selected term.
 */
void MainWindow::on_pushButtonSave_clicked()
{
    //Get the name of the currently-selected term
    //Use the term name to open its corresponding file
    QString currentTerm{ui->listWidgetEntries->currentItem()->text()};
    QFile file{currentTermFolder() + currentTerm};
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    //Get the edit-box conents
    //Store the contents into the term's file
    QString textEditContents{ui->textEdit->toPlainText()};
    QTextStream outStream(&file);
    outStream.setCodec("UTF-8");
    if (textEditContents != "" && textEditContents[0] != " ")
        outStream << textEditContents << "\n";

    file.flush();
    file.close();
}

/**
 * @brief MainWindow::on_pushButtonAdd_clicked
 * Adds a new term into the widget list.
 */
void MainWindow::on_pushButtonAdd_clicked()
{
    //Get the new term from the search-box
    //Check that them name is defined
    QString const lineEditContents{ui->lineEditSearch->text()};
    if (lineEditContents != "" && lineEditContents[0] != " ")
    {
        //Check that the file does does not exists or it will be overwritten
        QFile file{currentTermFolder() + lineEditContents};
        if(!file.exists())
        {
            //Create an empty file by opening it
            if (!file.open(QIODevice::WriteOnly))
                return;
            file.close();
        }
    }

    //Clear the widget list and reload it
    ui->listWidgetEntries->clear();
    loadTerms();
}

void MainWindow::on_listWidgetEntries_itemClicked()
{


    ui->pushButtonSave->setEnabled(true);
    ui->pushButtonDelete->setEnabled(true);

    /* Uses the text from the selected item
     * to find the file name that contains
     * the definition. The contents are then
     * displayed.
     */

    QString const currentTerm {ui->listWidgetEntries->currentItem()->text()};
    viewContents(currentTerm);
}

void MainWindow::on_comboBoxDictionaries_currentTextChanged()
{
    ui->listWidgetEntries->clear();
    loadTerms();
}

void MainWindow::on_pushButtonDelete_clicked()
{
    mDelete = new Delete{this};
    QObject::connect(mDelete,SIGNAL(accepted()), this, SLOT(deleteTerm()));
    mDelete->show();
}

void MainWindow::deleteTerm()
{
    QString listWidgetItem{ui->listWidgetEntries->currentItem()->text()};

        QFile file{currentTermFolder() + listWidgetItem};
        if(file.exists())
            file.remove();

    ui->listWidgetEntries->clear();
    loadTerms();
}

void MainWindow::on_lineEditSearch_textChanged(const QString &arg1)
{
    /* Uses the text from the selected item
     * to find the file name that contains
     * the definition. The contents are then
     * displayed.
     */

    QString const currentTerm {ui->lineEditSearch->text()};
    viewContents(currentTerm);

}

void MainWindow::viewContents(QString const &currentTerm)
{
    QFile file{currentTermFolder() + currentTerm};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;


    QByteArray contents{file.readAll()};
    file.close();

     ui->pushButtonSave->setEnabled(true);
     ui->textEdit->setPlainText(contents);

     QFile history{historyFile};
     if (!history.open(QIODevice::ReadWrite | QIODevice::Text))
         return;

     QTextStream inStream(&history);

     bool notInHistory = true;
     int entryNumber = 0;

     while (!inStream.atEnd())
     {
        if(inStream.readLine() == currentTerm)
            notInHistory = false;
        entryNumber++;
     }

     //this is a freaking mess
     int max = 50;
     if (entryNumber > max)
     {
         inStream.reset();
         for(int i = 0; i < max; i++)
         {

             QString s;
             QTextStream t(&history);
             while(!t.atEnd())
             {
                 QString line = t.readLine();
                 if(i > (max-30))
                     s.append(line + "\n");
             }
             history.resize(0);
             t << s;
             history.close();

         }
     }

     if (notInHistory)
     {
         QTextStream outStream(&history);
         outStream.setCodec("UTF-8");
         outStream << currentTerm << "\n";
     }



     history.flush();
     history.close();
}

void MainWindow::on_pushButtonBack_clicked()
{
    QFile history{historyFile};
    if (!history.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream outStream(&history);

    QList<QString> terms;
    while(!history.atEnd())
    {
        terms.push_back(history.readLine());
    }

}
