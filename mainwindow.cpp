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

QString const mainDirectory{"resources/"};
QString const historyDirectory{"resources/history.txt"};
QString MainWindow::currentDirectory() const
{
    QString currentDirectory{ui->comboBoxDictionaries->currentText()};
    return mainDirectory + currentDirectory + "/";
}

void MainWindow::loadDirectories()
{
    QDir const dir{mainDirectory};

    if (!dir.exists())
        dir.mkdir("../" + mainDirectory); //Go to the parent directory and create the maindDirectory folder

    for(QFileInfo item: dir.entryInfoList())
    {
        if (item.isDir() && item.baseName() != "")
            ui->comboBoxDictionaries->addItem(item.baseName());
            //Adding qPrintable(item.baseName()) is not neccessary, and additionally, it causes errors displaying russian characters.
    }
}

/**
 * @brief MainWindow::loadTerms
 * Loads the entries available into the widget list.
 * @param path the file directory where the entries
 * are stored
 */
void MainWindow::loadTerms()
{

    QDir dir{mainDirectory};

    QString const comboBoxDictionariesContents{ui->comboBoxDictionaries->currentText()};
    dir.cd(comboBoxDictionariesContents);

    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonSave->setEnabled(false);

    QStringList termList;


    for(QFileInfo item: dir.entryInfoList())
    {
        if (item.isFile())
        {
            ui->listWidgetEntries->addItem(item.fileName());
            termList << item.fileName();
        }

        //adding qPrintable(item.fileName()) is not neccessary, and additionally, it causes errors displaying russian characters.
    }

    //this piece takes care of text auto-completion
    mStringCompleter = new QCompleter(termList, this);
    mStringCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditSearch->setCompleter(mStringCompleter);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent}, ui{new Ui::MainWindow}
{
    ui->setupUi(this);
    loadDirectories();

    //Unables the Delete and Save buttons at load-up
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

void MainWindow::on_pushButtonSave_clicked()
{
    /* Uses the text from the selected item to
     * find the file cotaining the definition.
     */
    QString currentTerm{ui->listWidgetEntries->currentItem()->text()};
    QFile file{currentDirectory() + currentTerm};
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    /* Obtains the contents from the text edit
     * field and makes sure the field is not empty
     * before trying to write the difinition into
     * the file.
     */

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
 * Adds the word written inside of the line edit
 * field into the widget list.
 */
void MainWindow::on_pushButtonAdd_clicked()
{
    /* Obtains the contents fromt the line edit
     * field and makes sure the field is not empty
     * before creating a new file. The terms are
     * reloaded to include the newly created entry.
     */

    QString const lineEditContents{ui->lineEditSearch->text()};
    if (lineEditContents != "" && lineEditContents[0] != " ")
    {
        QFile file{currentDirectory() + lineEditContents};
        if(!file.exists()) //prevents files being overwritten
        {
            if (!file.open(QIODevice::WriteOnly)) //creates a file if it does not exist
                return;
            file.close();
        }
    }

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

        QFile file{currentDirectory() + listWidgetItem};
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
    QFile file{currentDirectory() + currentTerm};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;


    QByteArray contents{file.readAll()};
    file.close();

     ui->pushButtonSave->setEnabled(true);
     ui->textEdit->setPlainText(contents);

     QFile history{historyDirectory};
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
    QFile history{historyDirectory};
    if (!history.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream outStream(&history);

    QList<QString> terms;
    while(!history.atEnd())
    {
        terms.push_back(history.readLine());
    }

}
