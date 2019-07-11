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
QString const temporaryFile{"resources/temp.txt"};

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

/**
 * @brief MainWindow::on_listWidgetEntries_itemClicked
 * Enables the Save and Delete buttons for ther selected
 * term. Also displays the term's contents, if any.
 */
void MainWindow::on_listWidgetEntries_itemClicked()
{
    //Enable the Save and Delete buttons
    ui->pushButtonSave->setEnabled(true);
    ui->pushButtonDelete->setEnabled(true);

    //Get the current term name and view its contents
    QString const currentTerm{
        ui->listWidgetEntries->currentItem()->text()};
    viewContents(currentTerm);
}

/**
 * @brief MainWindow::on_comboBoxDictionaries_currentTextChanged
 * Loads the term folder whenever it is selected.
 */
void MainWindow::on_comboBoxDictionaries_currentTextChanged()
{
    //Clear the list widget and reload it
    ui->listWidgetEntries->clear();
    loadTerms();
}

void MainWindow::on_pushButtonDelete_clicked()
{
    mDelete = new Delete{this};
    QObject::connect(mDelete,SIGNAL(accepted()), this, SLOT(deleteTerm()));
    mDelete->show();
}

/**
 * @brief MainWindow::deleteTerm
 * Delets the selected term.
 */
void MainWindow::deleteTerm()
{
    //Get the selected term name and remove if it exists
    QString listWidgetItem{ui->listWidgetEntries->currentItem()->text()};
    QFile file{currentTermFolder() + listWidgetItem};
    if(file.exists())
        file.remove();

    //Clear the list widget and reload it
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

void MainWindow::viewContents(QString const &givenTerm)
{
    //Open the given term's file and store its contents
    QFile file{currentTermFolder() + givenTerm};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QByteArray contents{file.readAll()};
    file.close();

    //Load the contents and enable the Save button
    ui->textEdit->setPlainText(contents);
    ui->pushButtonSave->setEnabled(true);

    updateHistory(givenTerm);
}

/**
 * @brief MainWindow::updateHistory
 * Updates the history file and keeps track
 * of the terms that have been viewed.
 * @param givenTerm the term name of the viewed
 * widged list item
 */
void MainWindow::updateHistory(QString const &givenTerm)
{
    /*Function summary:
    If the given term exists and is the first one
    in the history file, then do nothing.

    If the given term exists but is not the
    first one in the history file,
    create a new temporary file,
    put the given term at the top of the list,
    place the rest of the terms beneath it,
    but remove any duplicates of the given term.
    Delete the original history file,
    and rename the temporary history file.

    If the given term does not exist,
    create new temporary file,
    put the given term at the top of the list,
    place the rest of the terms beneath it,
    Delete the original history file,
    and rename the temporary history file.
   */

    //Open the history file
    QFile history{historyFile};
    if (!history.open(QIODevice::ReadOnly))
        return;

    //Create an input stream for the history file
    //Read all the terms in history file
    //Encode toLocat8Bit to prevent cyrillic encoding erros
    //Close the hitory file
    QTextStream inStream{&history};
    QString contents{inStream.readAll().toLocal8Bit()};
    history.close();

    bool termExists{false};
    const int firstLine{0};
    int termLocation{0};
    int documentLength{0};
    for (QString row: contents.split("\r\n"))
    {
        //Check if the given term exists
        if(row == givenTerm)
        {
            termExists = true;
            //Indicate that the term is at top
            //of list when equal to 0
            termLocation = documentLength;
        }
    //Count the number of terms in the file
    documentLength++;
    }

    //Ignore if the term exists and
    //is the first one in the list
    if(termExists && termLocation == firstLine) {}

    else if(termExists && termLocation != firstLine)
    {
        //Create a temporary file
        QFile tempFile{temporaryFile};
        if(!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        //Create an out stream
        //Set the encoding to utf-8
        QTextStream outStream(&tempFile);
        outStream.setCodec("UTF-8");

        //Put the given term at the top of the list
        outStream << givenTerm << "\n";
        for (QString row: contents.split("\r\n"))
        {
            //Add the remaining terms into the list but remove
            //any duplicates of the given term
            if (row != givenTerm && row != "")
                outStream << row << "\n";
        }
        //Save the new temporary file
        tempFile.flush();
        tempFile.close();

        //Remove and replace the old history file
        history.remove();
        tempFile.rename(historyFile);
    }



    //Add the term if it does not exist
    else if(!termExists)
    {
        //Create a temporary file
        QFile tempFile{temporaryFile};
        if(!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        //Create an out stream
        //Set the encoding to utf-8
        QTextStream outStream(&tempFile);
        outStream.setCodec("UTF-8");

        //Put the given term at the top of the list
        outStream << givenTerm << "\n";
        for (QString row: contents.split("\r\n"))
        {
            //Add the remaining terms into the list
            if (row != "")
                outStream << row << "\n";
        }
        //Save the new temporary file
        tempFile.flush();
        tempFile.close();

        //Remove and replace the old history file
        history.remove();
        tempFile.rename(historyFile);
    }
    checkHistoryLength(documentLength);
}

/**
 * @brief MainWindow::checkHistoryLength
 * Checks the length of the history file and
 * ensures that it does not grow too large.
 * @param documentLength the current document
 * length represented by the number of lines
 * in the document
 */
void MainWindow::checkHistoryLength(const int &documentLength)
{
    //Save the las 20 terms whenever the
    //history goes beyond a max of 50 entries
    int const maxLength = 50;
    int const goodLength = 20;
    if (documentLength > maxLength)
    {
        //Create a temporary file
        //Create an input stream
        //Set the encoding to utf-8
        QFile temp{temporaryFile};
        if (!temp.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream inStream(&temp);
        inStream.setCodec("UTF-8");

        //Open the history file
        //Create an output file
        QFile history{historyFile};
        if (!history.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream outSream(&history);

        //Save the first 20 words and ignore the rest
        for (int i = 0; i < goodLength; i++)
            inStream << outSream.readLine().toLocal8Bit() << "\n";

        //Save and close the temporary file
        temp.flush();
        temp.close();

        //Remove and replace the old history file
        history.close();
        history.remove();
        temp.rename(historyFile);
    }
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
