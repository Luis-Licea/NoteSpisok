#ifndef DICTIONARIES_H
#define DICTIONARIES_H

#include <QDialog>

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
    void signalLoadTermFolders();

private slots:
    void on_pushButtonAdd_clicked();

private:
    Ui::Dictionaries *ui;
};

#endif // DICTIONARIES_H
