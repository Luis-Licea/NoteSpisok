#ifndef ABOUTAPP_H
#define ABOUTAPP_H

#include <QDialog>

namespace Ui {
class AboutApp;
}

class AboutApp : public QDialog
{
    Q_OBJECT

public:
    explicit AboutApp(QWidget *parent = nullptr);
    ~AboutApp();

private:
    Ui::AboutApp *ui;
};

#endif // ABOUTAPP_H
