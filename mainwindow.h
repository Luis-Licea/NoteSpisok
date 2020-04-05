#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "configuration.h"
#include "dictionaries.h"
#include "aboutapp.h"
#include "delete.h"
#include "rename.h"
#include <QListWidgetItem>
#include <QCompleter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    //Do not implement signals
    void relayTerm(QString);

private slots:
    void on_actionDictionaries_triggered();

    void on_actionConfiguration_triggered();

    void on_actionAboutApp_triggered();

    void on_actionExit_triggered();

    void on_pushButtonSave_clicked();

    void on_pushButtonAdd_clicked();

    void on_listWidgetEntries_itemClicked();

    void on_comboBoxDictionaries_currentTextChanged();

    void on_pushButtonDelete_clicked();

    void loadTerms();

    QString currentTermFolder(QString const &dictionary = NULL) const;

    void loadTermFolders();

    void deleteTerm();

    void checkHistoryLength(const int documentLength) const;

    void viewContents(QString const &currentTerm,
                      bool isCurrentItem,
                      bool historyUpdateNeeded,
                      bool savePreviousTermNeeded = false);

    void viewContents(QString const &termPath);

    void updateHistory(QString const &currentTerm) const;

    void renameTerm(QString const &newName);

    void on_lineEditSearch_textChanged();

    void getHistoryList(QList<QString> &terms);

    void on_pushButtonBack_clicked();

    void on_pushButtonNext_clicked();

    void on_pushButtonRename_clicked();

private:
    Ui::MainWindow *ui;
    Dictionaries *mDictionaries;
    Configuration *mConfiguration;
    AboutApp *mAboutApp;
    Delete *mDelete;
    QCompleter *mStringCompleter;
    Rename *mRename;

};

#endif // MAINWINDOW_H
