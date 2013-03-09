#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QKeyEvent>
#include <QTableView>
#include "keypresseater.h"
#include "gamemodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void keyPressEvent(QKeyEvent *);
private:
    GameModel *m_model;

    void init();
    void createActions();
    void createMenus();

    QTableView *gameTable;
    KeyPressEater *kpe;

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
