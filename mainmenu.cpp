#include "mainmenu.h"
#include "mainwindow.h"
#include <QApplication>

MainMenu::MainMenu(QWidget *parent) :
    QMenuBar(parent)
{
    QAction *newGameAct = new QAction(tr("Новая игра"), this);
    newGameAct->setShortcut(QKeySequence::New);
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));

    QAction *saveGameAct = new QAction(tr("Сохранить"), this);
    saveGameAct->setShortcut(QKeySequence::Save);
    connect(saveGameAct, SIGNAL(triggered()), this, SLOT(saveGame()));

    QMenu *gameMenu = addMenu("Игра");
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);

    QMenu *help = addMenu(tr("Help"));

}

void MainMenu::newGame()
{
    MainWindow *w = new MainWindow; // autorelease!
    w->show();
}

void MainMenu::saveGame()
{
  QMainWindow *w = reinterpret_cast<MainWindow*>(qApp->activeWindow());
  if (w) {
      w->setWindowTitle("hohohoh");
  }
}
