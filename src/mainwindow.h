#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QKeyEvent>
#include <QTableView>
#include "keypresseater.h"
#include "gamemodel.h"

#include "scannermanager.h"

class MainWindow : public QMainWindow, public ScannerManagerListener
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void onTextScanned(const QString &);
    
private:
    GameModel *m_model;

    void init();
    void createActions();
    void createMenus();

    QTableView *gameTable;
    QPalette originalPalette;
    KeyPressEater *kpe;

    QAction *newGameAct;
    QAction *saveGameAct;
    QAction *loadGameAct;
    QAction *closeGameAct;
    QAction *printBlanksAct;

    QAction *exportHTMLAct;
    QAction *exportCSVAct;

    QAction *addCommandAct;
    QAction *deleteCommandAct;

    QAction *questionCountAct;
    QTimer *timer;
signals:
    
public slots:
    void newGame();
    void loadGame();
    void saveGame();
    void printBlanks();

    void exportHTML();
    void exportCSV();

    void addCommand();
    void deleteCommand();

    void questionCount();

    void keyPressEvent(QKeyEvent *);
    void timerFinished();
    void closeEvent (QCloseEvent *event);
    void titleChanged();
};

#endif // MAINWINDOW_H
