#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include "delete.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("NoteSpisok");
    w.show();

    return a.exec();
}
