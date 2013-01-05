#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include "gamemodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    
private:
    GameModel *m_model;

    void init();
    void createActions();
    void createMenus();

    QAction *newGameAct;
    QAction *saveGameAct;
    QAction *loadGameAct;
    QAction *closeGameAct;
    QAction *printBlanksAct;

    QAction *addCommandAct;
    QAction *deleteCommandAct;

    QAction *questionCountAct;
signals:
    
public slots:
    void newGame();
    void loadGame();
    void saveGame();
    void printBlanks();

    void addCommand();
    void deleteCommand();

    void questionCount();
};

#endif // MAINWINDOW_H
