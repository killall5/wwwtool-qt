#include <QApplication>
#include "mainwindow.h"
#include <QDebug>
#include <QVector>
#include <QString>

#include <QCryptographicHash>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow;
    w->show();

    return a.exec();
}
