#ifndef DICTIONARIES_H
#define DICTIONARIES_H

#include <QDialog>
#include "rename.h"
#include "delete.h"

namespace Ui {
class Dictionaries;
}

class Dictionaries : public QDialog
{
    Q_OBJECT

public:
    explicit Dictionaries(QWidget *parent = nullptr);
    ~Dictionaries();

    void loadTermFolders();

signals:
    void relayDictionary(QString);

    void signalLoadTermFolders();

private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonRename_clicked();

    void renameDictionary(QString const &newName);

    void deleteDictionary();

private:
    Ui::Dictionaries *ui;
    Rename *mRename;
    Delete *mDelete;
};

#endif // DICTIONARIES_H
