#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "configuration.h"
#include "dictionaries.h"
#include "aboutapp.h"
#include "delete.h"
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

    //Do not implement signals
signals:
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

    QString currentTermFolder() const;

    void loadTermFolders();

    void deleteTerm();

    void checkHistoryLength(const int &);

    void viewContents(QString const &);

    void updateHistory(QString const &);

    void on_lineEditSearch_textChanged(const QString &arg1);

    void on_pushButtonBack_clicked();

private:
    Ui::MainWindow *ui;
    Dictionaries *mDictionaries;
    Configuration *mConfiguration;
    AboutApp *mAboutApp;
    Delete *mDelete;
    QCompleter *mStringCompleter;

};

#endif // MAINWINDOW_H
