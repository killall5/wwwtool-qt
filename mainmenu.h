#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenuBar>

class MainMenu : public QMenuBar
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = 0);
    
signals:
    
public slots:
    void newGame();
    void saveGame();
    void printBlanks();

private:


};

#endif // MAINMENU_H
