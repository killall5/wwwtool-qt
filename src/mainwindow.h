#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "gamemodel.h"

class QMediaPlayer;
class QAction;
class QComboBox;
class QKeyEvent;
class QTableView;
class QMenuBar;
class QItemSelection;
class QLineEdit;
class QTimer;
class QString;
class QLabel;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    GameModel *m_model;

    void createActions();
    void createMenus();

    QTableView *gameTable;
    QPalette originalPalette;

    QAction *newGameAct;
    QAction *saveGameAct;
    QAction *loadGameAct;
    QAction *closeGameAct;
    QAction *finishedGameAct;
    QAction *printBlanksAct;
    QAction *printTablesAct;

    QAction *exportHTMLAct;
    QAction *exportCSVAct;

    QAction *addCommandAct;
    QAction *deleteCommandAct;
    QAction *sortByTitleAct;
    QAction *sortByTableAct;
    QAction *sortByResultAct;

    QAction *questionCountAct;
    QMediaPlayer *player;
    QMenuBar *menuBar;
    QComboBox *fixedQuestionBox;
    QLabel *helperLabel;
    QLineEdit *helperLineEdit;
    QTimer *timer;

    void filterCommands(const QString& value);
    void processScannedText(const QString& value);
signals:
    
public slots:
    void newGame();
    void loadGame();
    void saveGame();
    void changeFinished();
    void printBlanks();
    void printTables();

    void exportHTML();
    void exportCSV();

    void addCommand();
    void deleteCommand();
    void sortByTitle();
    void sortByTable();
    void sortByResult();

    void questionCount();

    void timerFinished();

    void keyPressEvent(QKeyEvent *);
    void closeEvent (QCloseEvent *event);
    void titleChanged();
    void finishedChanged();
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void fixQuestion(int);
    void questionCountChanged(int);
    void enterKeyPressed();
};

#endif // MAINWINDOW_H
