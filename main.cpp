#include <QApplication>
#include "mainwindow.h"
#include <QDebug>

#ifdef Q_OS_MAC
#include "mainmenu.h"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef Q_OS_MAC
    a.setQuitOnLastWindowClosed(false);
    MainMenu menu(0);
#endif
    MainWindow *w = new MainWindow;
    w->show();

    return a.exec();
}
