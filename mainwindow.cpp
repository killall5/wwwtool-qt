#include "mainwindow.h"
#include <QTableWidget>
#include <QTableView>
#include <QtDebug>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include "addcommanddialog.h"
#include "questioncountdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_model = new GameModel(this);

    QTableView *gameTable = new QTableView(this);
    gameTable->setModel(m_model);

    setCentralWidget(gameTable);

    createActions();
    createMenus();
    resize(700, 500);
}

void MainWindow::createActions()
{
    newGameAct = new QAction(tr("Новая игра"), this);
    newGameAct->setShortcut(QKeySequence::New);
    connect(newGameAct, SIGNAL(triggered()), this, SLOT(newGame()));

    saveGameAct = new QAction(tr("Сохранить"), this);
    saveGameAct->setShortcut(QKeySequence::Save);
    connect(saveGameAct, SIGNAL(triggered()), this, SLOT(saveGame()));

    loadGameAct = new QAction(tr("Открыть..."), this);
    loadGameAct->setShortcut(QKeySequence::Open);
    connect(loadGameAct, SIGNAL(triggered()), this, SLOT(loadGame()));

    closeGameAct = new QAction(tr("Закрыть"), this);
    closeGameAct->setShortcut(QKeySequence::Close);
    connect(closeGameAct, SIGNAL(triggered()), this, SLOT(close()));

    printBlanksAct = new QAction(tr("Бланки ответов..."), this);
    printBlanksAct->setShortcut(QKeySequence::Print);
    connect(printBlanksAct, SIGNAL(triggered()), this, SLOT(printBlanks()));

    addCommandAct = new QAction(tr("Добавить команду..."), this);
    connect(addCommandAct, SIGNAL(triggered()), this, SLOT(addCommand()));

    deleteCommandAct = new QAction(tr("Удалить команду..."), this);
    connect(deleteCommandAct, SIGNAL(triggered()), this, SLOT(deleteCommand()));

    questionCountAct = new QAction(tr("Количество вопросов..."), this);
    questionCountAct->setShortcut(tr("Ctrl+E"));
    connect(questionCountAct, SIGNAL(triggered()), this, SLOT(questionCount()));
}

void MainWindow::createMenus()
{
    QMenu *gameMenu = menuBar()->addMenu("Игра");
    gameMenu->addAction(newGameAct);
    gameMenu->addAction(saveGameAct);
    gameMenu->addAction(loadGameAct);
    gameMenu->addAction(closeGameAct);
    gameMenu->addSeparator();
    gameMenu->addAction(printBlanksAct);

    QMenu *commandsMenu = menuBar()->addMenu("Команды");
    commandsMenu->addAction(addCommandAct);
    commandsMenu->addAction(deleteCommandAct);

    QMenu *questionsMenu = menuBar()->addMenu("Вопросы");
    questionsMenu->addAction(questionCountAct);

//    QMenu *help = menuBar()->addMenu(tr("Help"));


}

void MainWindow::newGame()
{
    MainWindow *w = new MainWindow();
    w->show();
}

void MainWindow::loadGame()
{

}

void MainWindow::saveGame()
{

}

void MainWindow::printBlanks()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export blanks"), QString(), tr("PDF (*.pdf)"));

    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    m_model->printBlanks(&printer);
}

void MainWindow::addCommand()
{
    AddCommandDialog *addCommandDialog = new AddCommandDialog(m_model, this);
//    connect()
    addCommandDialog->exec();
}

void MainWindow::deleteCommand()
{

}

void MainWindow::questionCount()
{
    QuestionCountDialog dialog(m_model, this);
    dialog.exec();
}
