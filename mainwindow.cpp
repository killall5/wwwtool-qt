#include "mainwindow.h"
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QTableWidget *gameTable = new QTableWidget;
    gameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    gameTable->setRowCount(3);
    gameTable->setColumnCount(4);


    setCentralWidget(gameTable);
}
